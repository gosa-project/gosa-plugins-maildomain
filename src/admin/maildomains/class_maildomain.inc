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

class maildomain extends plugin
{
    // Variables
    public $ou= "";
    public $associatedDomain = "";
    public $description= "";
    public $businessCategory= [];
    public $businessCategoryUnknown= false;

    // The object's base
    public $base= "";

    // Keep track of possible naming changes
    public $orig_dn= "";
    public $orig_ou= "";
    public $orig_base= "";

    public $view_logged = false;

    public $baseSelector;

    var $theme;

    // Object classes written by this plugin
    public $objectclasses= array("top", "organizationalUnit", "domainRelatedObject");

    // A list of attributes managed by this class
    public $attributes= array("ou", "associatedDomain", "description", "businessCategory");


    public function __construct(&$config, $dn= null)
    {
        $this->theme = getThemeName();
        switch ($this->theme) {
            case 'classic':
                $this->typeToImage = array(
                    "D" => "plugins/departments/images/domain.png",
                );
                break;

            default:
                $this->typeToImage = array(
                    "D" => "<i class='material-icons'>domain</i>",
                );
                break;
        }

        plugin::__construct($config, $dn);

        $this->orig_dn= $dn;

        $this->is_account= true;

        // Set base
        if ($this->dn == "new") {
            $ui = get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"ou=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
        } else {
            $this->base= preg_replace("/^[^,]+,".preg_quote(get_ou("maildomain", "maildomainRDN"), '/')."/i", "", $this->dn);
        }

        $this->orig_ou = $this->ou;
        $this->orig_base = $this->base;

        // Instanciate base selector
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);
    }


    public function execute()
    {
        // Call parent execute
        plugin::execute();

        if (!$this->view_logged) {
            $this->view_logged = true;
            new log("view", "maildomains/".get_class($this), $this->dn);
        }

        // Do we represent a valid maildomain?
        if (!$this->is_account) {
            switch ($this->theme) {
                case 'classic':
                  $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
                  break;
                default:
                  $symbol = "<i class='material-icons'>report</i>";
                  break;
            }
            $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("mail domain"))."</div>";
            return ($display);
        }

        // Load templating engine
        $smarty= get_smarty();

        $tmp = $this->plInfo();
        foreach ($tmp['plProvidedAcls'] as $name => $translation) {
            $smarty->assign($name."ACL", $this->getacl($name));
        }

        /* Get list of set tags (via businessCategory) */
        if (isset($this->attrs['businessCategory'])) {
            $this->businessCategory = array();
            for ($i = 0; $i < $this->attrs['businessCategory']['count']; $i++) {
                if (trim($this->attrs['businessCategory'][$i]) !== "") {
                    $this->businessCategory[] = trim($this->attrs['businessCategory'][$i]);
                }
            }
        }

        // Assign variables
        $smarty->assign("bases", $this->get_allowed_bases());
        $smarty->assign("base_select", $this->base);

        $maildomainActive  = "";
        $maildomainRetired = "";
        $maildomainUnknown = "";
        if (in_array("ACTIVE", $this->businessCategory)) {
            $maildomainActive  = "checked";
        }
        else if (in_array("RETIRED", $this->businessCategory)) {
            $maildomainRetired = "checked";
        }
        if (in_array("UNKNOWN", $this->businessCategory)) {
            $maildomainUnknown = "checked";
        }
        $smarty->assign("maildomainActive_checked",  $maildomainActive);
        $smarty->assign("maildomainRetired_checked", $maildomainRetired);
        $smarty->assign("maildomainUnknown_checked", $maildomainUnknown);

        // Fields
        foreach ($this->attributes as $val) {
            $smarty->assign("$val", set_post($this->$val));
        }

        return ($smarty->fetch(get_template_path('maildomain.tpl', true)));
    }


    public function set_acl_base($base)
    {
        plugin::set_acl_base($base);
    }


    // Save data to object
    public function save_object()
    {
        // Sync ou value to associatedMailDomain value...
        $this->associatedDomain = $this->ou;

        // Process mail domain categories
        $businessCategory = array();

        if (isset($_POST['businessCategory'])) {
            $bc = get_post('businessCategory');
            if (is_array($bc)) {
                $businessCategory = $bc;
            } else {
                $businessCategory[] = $bc;
            }
        }
        if (isset($_POST['businessCategoryUnknown'])) {
            $bcu = get_post('businessCategoryUnknown');
            if (!in_array($bcu, $businessCategory)) {
                $businessCategory[] = $bcu;
            }
        }

        parent::save_object();

        $this->businessCategory = set_post($businessCategory);

        if (isset($_POST['base'])) {
            $this->base = get_post('base');
        }
    }


    // Validate object data
    public function check()
    {
        // Call common method to give check the hook
        $message= plugin::check();

        // Permissions for that base?
        if ($this->base != "") {
            $new_dn= 'ou='.$this->ou.','.get_ou("maildomain", "maildomainRDN").$this->base;
        } else {
            $new_dn= $this->dn;
        }

        $ldap = $this->config->get_ldap_link();
        if (LDAP::fix($this->dn) != LDAP::fix($new_dn)) {
            $ldap->cat($new_dn, array('dn'));
        }

        if ($ldap->count() !=0) {
            $message[]= msgPool::duplicated(_("Mail Domain"));
        }

        // Check if a wrong base was supplied
        if (!$this->baseSelector->checkLastBaseUpdate()) {
            $message[]= msgPool::check_base();
            ;
        }

        // Set new acl base
        if ($this->dn == "new") {
            $this->set_acl_base($this->base);
        }

        // Check if we are allowed to create/move this user
        if ($this->orig_dn == "new" && !$this->acl_is_createable($this->base)) {
            $message[] = msgPool::permCreate();
        } elseif (
            $this->orig_dn != "new" &&
            !$this->acl_is_moveable($this->base) &&
            ($this->orig_base != $this->base || $this->orig_cn != $this->cn)
        ) {
            $message[] = msgPool::permMove();
        }

        // must: ou
        if ($this->ou == "") {
            $message[]= msgPool::required(_("Mail domain"));
        }

        if (preg_match('/[=,+<>#;]/', $this->ou)) {
            $message[] = msgPool::invalid(_("Mail domain"), $this->ou, "/[^=+,<>#;]/");
        }

        // Check if we are allowed to create or move this object
        if ($this->orig_dn == "new" && !$this->acl_is_createable($this->base)) {
            $message[] = msgPool::permCreate();
        } elseif ($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)) {
            $message[] = msgPool::permMove();
        }

        return ($message);
    }


    // Save to LDAP
    public function save()
    {
        plugin::save();

        $ldap= $this->config->get_ldap_link();

        // New accounts need proper 'dn', propagate it to remaining objects
        if ($this->dn == 'new') {
            $this->dn= 'ou='.$this->ou.','.get_ou("maildomain", "maildomainRDN").$this->base;
        }

        // Save data. Using 'modify' implies that the entry is already present, use 'add' for
        // new entries. So do a check first...
        $ldap->cat($this->dn, array('dn'));
        if ($ldap->fetch()) {
            $mode= "modify";
        } else {
            $mode= "add";
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        }

        // Write back to ldap
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->$mode($this->attrs);

        if ($mode == "add") {
            new log("create", "maildomains/".get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        } else {
            new log("modify", "maildomains/".get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        }

        // Trigger post signal
        $this->handle_post_events($mode);

        $ret= 0;
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
            $ret= 1;
        }

        return ($ret);
    }

    public function remove_from_parent()
    {
        plugin::remove_from_parent();

        $ldap= $this->config->get_ldap_link();
        $ldap->rmdir($this->dn);
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }

        new log("remove", "maildomains/".get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());

        // Trigger remove signal
        $this->handle_post_events("remove");
    }


    public static function plInfo()
    {
        $properties = maildomain::getProperties();

        return (array(
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Mail domain generic"),
                    "plSelfModify"  => false,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("administration"),
                    "plOptions"     => array(),
                    "plProperties"  => $properties,
                    "plRequirements"=> array(
                        'ldapSchema' => array('domainRelatedObject' => ''),
                        'onFailureDisablePlugin' => array(get_class(), 'maildomainManagement')
                        ),
                    "plCategory"    => array(
                        'maildomains' => array(
                            'description'  => _('Mail domains'),
                            'objectClass'  => 'domainRelatedObject',
                        )
                    ),
                    "plProvidedAcls"=> array(
                        "ou"                => _("Organizational Unit"),
                        "associatedDomain"  => _("Mail Domain"),
                        "description"       => _("Description"),
                        "base"              => _("Base"),
                        "businessCategory"  => _("Category"),
                    ),
        ));
    }

    /*
     * Define properties (saved in LDAP as 'gosaSettings' or in gosa.conf).
     * @return array Properties information.
     */
    public static function getProperties()
    {
        return array(
            array(
                "name"        => "maildomainRDN",
                "type"        => "string",
                "check"       => "gosaProperty::isString",
                "group"       => "core",
                "default"     => "ou=maildomains",
                "migrate"     => "",
                "mandatory"   => true,
                "description" => _("RDN for maildomains."),
            ),
        );
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
