<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class maildomainManagement extends management
{
    var $plHeadline     = "Mail Domains";
    var $plDescription  = "Mail domain management.";
    var $plIcon         = "plugins/departments/images/domain.png";
    var $matIcon        = "domain";

    // Tab definition
    protected $tabClass = "maildomaintabs";
    protected $tabType = "MAILDOMAINTABS";
    protected $aclCategory = "maildomains";
    protected $aclPlugin   = "maildomain";
    protected $objectName   = "maildomain";

    function __construct($config, $ui)
    {
        $this->config = $config;
        $this->ui = $ui;

        $this->maildomainBaseOnly = ($config->get_cfg_value('core', 'maildomainBaseOnly') == "true");
        $this->storagePoints = array(get_ou("maildomain", "maildomainRDN"));

        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("maildomain-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        if ($this->maildomainBaseOnly) {
            $headpage = new listing(get_template_path("maildomain-list-baseonly.xml", true));
        } else {
            $headpage = new listing(get_template_path("maildomain-list.xml", true));
        }
        $headpage->registerElementFilter("maildomainCategories",   "maildomainManagement::maildomainCategories");
        $headpage->registerElementFilter("maildomainUsageCounter", "maildomainManagement::maildomainUsageCounter");
        $headpage->setFilter($filter);
        $headpage->update();

        // Add snapshot handler.
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }

        parent::__construct($config, $ui, "maildomains", $headpage);
    }


   /*! \brief  Initiates the removal for the given entries
     *         and displays a confirmation dialog.
     *
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    protected function removeEntryRequested($action = "", $target = array(), $all = array())
    {
        // Close dialogs and remove locks for currently handled dns
        $this->cancelEdit();

        $disallowed = array();
        $this->dns = array();

        DEBUG(DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $target, "Entry removel requested!");

        // Check permissons for each target
        $h = $this->getHeadpage();
        $oTypes = array_reverse($h->objectTypes);
        foreach ($target as $dn) {
            $entry = $h->getEntry($dn);
            $obj   = $h->getObjectType($oTypes, $entry['objectClass']);
            $acl = $this->ui->get_permissions($dn, $obj['category'] . "/" . $obj['class']);
            if (preg_match("/d/", $acl)) {
                $this->dns[] = $dn;
            } else {
                $disallowed[] = $dn;
            }
        }
        if (count($disallowed)) {
            msg_dialog::display(_("Permission"), msgPool::permDelete($disallowed), INFO_DIALOG);
        }

        // We've at least one entry to delete.
        if (count($this->dns)) {

            // check locks
            if ($user = get_multiple_locks($this->dns)) {
                return (gen_locked_message($user, $this->dns));
            }

            // Add locks
            $dns = array();
            $dns_subobjects = array();

            $ldap = $this->config->get_ldap_link();

            foreach ($this->dns as $dn) {
                $dns[] = LDAP::fix($dn);
                $ldap->cd($dn);
                $dns_subobjects[$dn] = array();
                $srp = $ldap->search('(&(objectClass=*)(!(associatedDomain=*)))', array('dn'));
                while ($attrs = $ldap->fetch($srp)) {
                    $dns_subobjects[$dn][] = LDAP::fix($attrs['dn']);
                }
            }

            add_lock($this->dns, $this->ui->dn);

            // Display confirmation dialog.
            $smarty = get_smarty();
            $smarty->assign("dns", $dns);
            $smarty->assign("dns_subobjects", $dns_subobjects);
            return ($smarty->fetch(get_template_path('maildomain-removeEntries.tpl', true)));
        }
    }

    /*! \brief  Object removal was confirmed, now remove the requested entries.
     *
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    public function removeEntryConfirmed(
        $action = "",
        $target = array(),
        $all = array(),
        $altTabClass = "",
        $altTabType = "",
        $altAclCategory = "",
        $altAclPlugin = ""
    ) {
        $tabType = $this->tabType;
        $tabClass = $this->tabClass;
        $aclCategory = $this->aclCategory;
        $aclPlugin = $this->aclPlugin;
        if (!empty($altTabClass)) {
            $tabClass = $altTabClass;
        }
        if (!empty($altTabType)) {
            $tabType = $altTabType;
        }
        if (!empty($altAclCategory)) {
            $aclCategory = $altAclCategory;
        }
        if (!empty($altAclPlugin)) {
            $aclPlugin = $altAclPlugin;
        }

        DEBUG(DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $target, "Entry removel confirmed!");

        // Check permissons for each target
        $h = $this->getHeadpage();
        $oTypes = array_reverse($h->objectTypes);
        foreach ($this->dns as $key => $dn) {
            $entry = $h->getEntry($dn);
            $obj   = $h->getObjectType($oTypes, $entry['objectClass']);
            $acl = $this->ui->get_permissions($dn, $obj['category'] . "/" . $obj['class']);

            // Check permissions, are we allowed to remove this object?
            if (preg_match("/d/", $acl)) {

                // Delete subobjects under ou=requests recursively (there should be no others)
                $ldap = $this->config->get_ldap_link();
                $ldap->cd($dn);
                $srp = $ldap->search('(&(objectClass=organizationalUnit)(!(associatedDomain=*))(ou=requests))', array('dn'));
                if ($ldap->count($srp) === 1) {
                    $attrs = $ldap->fetch($srp);
                    $ldap->rmdir_recursive(LDAP::fix($attrs['dn']));
                }

                // Delete the object
                $this->dn = $dn;
                $this->tabObject = new $tabClass(
                    $this->config,
                    $this->config->data['TABS'][$tabType],
                    $this->dn,
                    $aclCategory,
                    true,
                    true
                );
                $this->tabObject->set_acl_base($this->dn);
                $this->tabObject->parent = &$this;
                $this->tabObject->delete();

                // Remove the lock for the current object.
                del_lock($this->dn);
            } else {
                msg_dialog::display(_("Permission error"), msgPool::permDelete(), ERROR_DIALOG);
                new log("security", "groups/" . get_class($this), $dn, array(), "Tried to trick deletion.");
            }
        }

        // Cleanup
        $this->remove_lock();
        $this->closeDialogs();
    }


    static function maildomainUsageCounter($dn, $associatedDomain)
    {

        $base_of_maildomain = preg_replace ('/^[^,]+,[^,]+,/i', '', $dn);
        return(maildomain::maildomain_count_usage($associatedDomain[0], $base_of_maildomain));
    }

    static function maildomainCategories($dn, $associatedDomain)
    {
        $base_of_maildomain = preg_replace ('/^[^,]+,[^,]+,/i', '', $dn);
        return(maildomain::maildomain_categories($associatedDomain[0], $base_of_maildomain));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
