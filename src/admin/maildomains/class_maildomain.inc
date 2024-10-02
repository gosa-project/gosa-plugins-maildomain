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

// variables | code path influencers

global $maildomain_count_users;
global $maildomain_count_usage;
global $maildomain_count_usage_phpldapsearch;
global $maildomain_count_usage_externalscript;
$maildomain_count_users = FALSE;
$maildomain_count_usage = !$maildomain_count_users;
$maildomain_count_usage_phpldapsearch  = FALSE;
$maildomain_count_usage_externalscript = !$maildomain_count_usage_phpldapsearch;

global $maildomain_count_usage_phpldapsearch_counterlimit;
// Not recommended to go beyond 100 search result array-deep-divings (see below)
$maildomain_count_usage_phpldapsearch_counterlimit  = 100;

global $maildomain_category_map;
$maildomain_category_map = [
    "ACTIVE"  => _("active"),
    "RETIRED" => _("retired"),
    "UNKNOWN" => _("unknown"),
];

class maildomain extends plugin
{
    // Variables
    public $ou= "";
    public $associatedDomain = "";
    public $description= "";
    public $businessCategory= [];
    public $businessCategoryUnknown= false;

    public $countMaildomainUsage;

    // The object's base
    public $base= "";

    // Keep track of possible naming changes
    public $orig_dn= "";
    public $orig_ou= "";
    public $orig_base= "";

    public $view_logged = false;

    public $baseSelector;
    public $maindomainBaseOnly= false;

    var $theme;

    // Object classes written by this plugin
    public $objectclasses= array("top", "organizationalUnit", "domainRelatedObject");

    // A list of attributes managed by this class
    public $attributes= array("ou", "associatedDomain", "description", "businessCategory");


    public $readonly_attributes = array(
        "createTimestamp", "modifyTimestamp"
    );


    public function __construct(&$config, $dn= null)
    {
        $this->theme = getThemeName();

        plugin::__construct($config, $dn);

        $this->orig_dn= $dn;

        $this->is_account= true;

        // Get read only attributes.
        // For some reason plugin::plugin doesn't fetch
        // attributes like 'modifyTimestamp'. So we've to
        // fetch them manually.
        $ldap = $config->get_ldap_link();
        $ldap->cat($this->dn, $this->readonly_attributes);
        if ($ldap->count()) {
            $attrs = $ldap->fetch();
            foreach ($this->readonly_attributes as $attr) {
                if (isset($attrs[$attr])) {
                    $this->$attr = $attrs[$attr][0];
                }
            }
        }

        // Get list of set tags (via businessCategory)
        if (isset($this->attrs['businessCategory'])) {
            $this->businessCategory = array();
            for ($i = 0; $i < $this->attrs['businessCategory']['count']; $i++) {
                if (trim($this->attrs['businessCategory'][$i]) !== "") {
                    $this->businessCategory[] = trim($this->attrs['businessCategory'][$i]);
                }
            }
        }

        // maildomains at BaseDN only?
        $this->maildomainBaseOnly = ($config->get_cfg_value('core', 'maildomainBaseOnly') == "true");

        // Set base
        if ($this->dn == "new") {
            $ui = get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"ou=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
        } else {
            $this->base= preg_replace("/^[^,]+,".preg_quote(get_ou("maildomain", "maildomainRDN"), '/')."/i", "", $this->dn);
            $this->countMaildomainUsage = maildomain::maildomain_count_usage($this->associatedDomain, $this->base);
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
        $smarty->assign($name."ACL", $this->getacl($name));

        if ($this->dn == "new") {
            $this->businessCategory = array();
            $this->businessCategory[] = "ACTIVE";
        } else {
            /* Get list of set tags (via businessCategory) */
            if (isset($this->attrs['businessCategory'])) {
                $this->businessCategory = array();
                for ($i = 0; $i < $this->attrs['businessCategory']['count']; $i++) {
                    if (trim($this->attrs['businessCategory'][$i]) !== "") {
                        $this->businessCategory[] = trim($this->attrs['businessCategory'][$i]);
                    }
                }
            }
        }

        // Assign variables

        if (!$this->maildomainBaseOnly) {
            $smarty->assign("base", $this->baseSelector->render());
        } else {
            $smarty->assign("base", NULL);
        }

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

        if ($this->dn != "new") {
            // read-only attributes
            foreach ($this->readonly_attributes as $attr) {
                if (str_ends_with($attr, "Timestamp")) {
                    $smarty->assign($attr, date("d.m.Y H:i:s", strtotime($this->$attr)));
                } else {
                    $smarty->assign($attr, $this->$attr);
                }
            }
        }

        // How many users have a mailaddress in this mail domain?
        if ($this->dn !== 'new') {
            $this->countMaildomainUsage = maildomain::maildomain_count_usage($this->associatedDomain, $this->base);
        }
        $smarty->assign("countMaildomainUsage", $this->countMaildomainUsage);
        $smarty->assign("countMaildomainUsageACL", $this->getacl("countMaildomainUsage"));

        // Is new?
        $smarty->assign("is_new", $this->dn == "new" ? "true" : "false");

        return ($smarty->fetch(get_template_path('maildomain.tpl', true)));
    }


    public function set_acl_base($base)
    {
        plugin::set_acl_base($base);
    }


    public function acl_is_moveable($base = "")
    {
        if ($this->maildomainBaseOnly && ($base != $this->config->current['BASE'])) {
            return FALSE;
        }

        return parent::acl_is_moveable($base);
    }


    public function acl_is_createable($base = "")
    {
        if ($this->maildomainBaseOnly && ($base != $this->config->current['BASE'])) {
            return FALSE;
        }

        return parent::acl_is_createable($base);
    }


    public function acl_is_removeable($base = "")
    {
        if ($this->countMaildomainUsage > 0) {
            return FALSE;
        }
        return parent::acl_is_removeable($base);
    }


    // Save data to object
    public function save_object()
    {
        if (
            (($this->dn == "new") && ($this->acl_is_createable())) ||
            ($this->acl_is_writeable("ou"))
           )
        {
            // Sync ou value to associatedMailDomain value...
            $this->associatedDomain = $this->ou;
        }

        parent::save_object();

        // Process mail domain categories
        $businessCategory = array();

        if ($this->acl_is_writeable("businessCategory")) {
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

            // sort and array and make values unique
            sort($businessCategory);
            $businessCategory = array_unique($businessCategory);

            $this->businessCategory = set_post($businessCategory);
        }

        if (isset($_POST['base'])) {
            $this->base = get_post('base');
        }
    }


    // Validate object data
    public function check()
    {
        global $maildomain_category_map;

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

        // businessCategory: ACTIVE, RETIRED or UNKNOWN
        foreach($this->businessCategory as $key => $val) {
            if (! in_array($val, array_keys($maildomain_category_map)))
            {
                $message[]= (_("Unsupported category found in mail domain categories, your POST request has been compromised."));
                break;
            }
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

        // Create ou=requests sub-ou for storing mail account applications / requests if it does not yet exist...
        $ldap->search('ou=requests');
        if ($ldap->count() < 1) {
            $requests_ou_attrs = array(
                'ou' => 'requests',
                'objectClass' => array('top', 'organizationalUnit'),
                'description' => sprintf(_("Mail account requests for mail domain '%s'"), $this->associatedDomain),
            );
            $ldap->cd('ou=requests,ou='.$this->ou.','.get_ou("maildomain", "maildomainRDN").$this->base);
            $ldap->add($requests_ou_attrs);
            $ldap->cd($this->dn);

            new log("create", "maildomains/".get_class($this), "ou=requests,".$this->dn, array_keys($requests_ou_attrs), $ldap->get_error());
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


    public function allow_remove()
    {
        // Don't allow removal if mail addresses still use this mail domain.
        if ($this->countMaildomainUsage != 0) {
            return (sprintf(_("Mail domain '%s' can't be removed, because %d mail addresses are still configured for this domain."), $this->associatedDomain, $this->countMaildomainUsage));
        }
        return ("");
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
                        "ou"                   => _("Mail domain"),
                        "description"          => _("Description"),
                        "base"                 => _("Base"),
                        "businessCategory"     => _("Category"),
                        "createTimestamp"      => _("Creation Timestap (read-only)"),
                        "modifyTimestamp"      => _("Modification Timestap (read-only)"),
                        "countMaildomainUsage" => _("Mail addresses used in this domain (read-only)"),
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
            array(
                "name"        => "maildomainBaseOnly",
                "type"        => "bool",
                "check"       => "gosaProperty::isBool",
                "group"       => "core",
                "default"     => FALSE,
                "migrate"     => "",
                "mandatory"   => true,
                "description" => _("Only allow maildomains at top-level BaseDN."),
            ),
        );
    }


    public static function maildomain_categories($md, $base)
    {
        global $config;
        global $maildomain_category_map;

        $ldap = $config->get_ldap_link();
        $ldap->cd($base);
        $search_filter = "(&(objectClass=organizationalUnit)(objectClass=domainRelatedObject)(associatedDomain=".$md."))";
        $srp = $ldap->search($search_filter, array("businessCategory"));

        $attrs = $ldap->fetch($srp);

        $categories       = [];
        $categories_short = [];

        if (in_array("businessCategory", $attrs)) {
            $i_max = $attrs["businessCategory"]["count"];
            for ($i = 0; $i < $i_max; $i++) {
                if ($attrs["businessCategory"][$i][0] !== NULL) {
                    $categories[]       = $maildomain_category_map[$attrs["businessCategory"][$i]];
                    $categories_short[] = strtoupper($maildomain_category_map[$attrs["businessCategory"][$i]])[0];
                }
            }
        }
        return ("<div title=\"".join(", ", $categories)."\">".join("|", $categories_short)."</div>");
    }

    public static function maildomain_count_usage($md, $base)
    {
        global $config;
        global $BASE_DIR;
        global $maildomain_count_usage;
        global $maildomain_count_users;
        global $maildomain_count_usage_phpldapsearch;
        global $maildomain_count_usage_phpldapsearch_counterlimit;
        global $maildomain_count_usage_externalscript;

        if ($maildomain_count_users) {

            // Only count LDAP users using a mail address in the given mail domain

            // Search LDAP tree for mail / alias / mailAlias attributes using
            // this object's associatedDomain and detect how many accounts have
            // a mail address in this domain.
            $ldap = $config->get_ldap_link();
            $ldap->cd($base);

            $search_filter = "(&(objectClass=gosaAccount)(|(mail=*@".$md.")(alias=*@".$md.")(mailAlias=*@".$md.")))";
            $ldap->search($search_filter, array("uid"));
            return ($ldap->count());

        } elseif ($maildomain_count_usage) {

            // Count mail addresses defined in the given mail domain

            if ($maildomain_count_usage_phpldapsearch) {

                // Use GOsa²'s class_ldap and deep-dive into the result array (SLOOOWWWWW...)

                // Search LDAP tree for mail / alias / mailAlias attributes using
                // this object's associatedDomain and detect how many accounts have
                // a mail address in this domain.
                $ldap = $config->get_ldap_link();
                $ldap->cd($base);

                $counter = 0;
                $counter_limit = $maildomain_count_usage_phpldapsearch_counterlimit;

                $search_attrs = array("mailAlias", "mail", "alias");
                foreach ($search_attrs as $idx => $attr) {

                    $search_filter = "(&(objectClass=gosaAccount)(".$attr."=*@".$md."))";
                    $srp = $ldap->search($search_filter, array($attr));

                    // retrieve individual objects of the search result
                    while ($attrs = $ldap->fetch($srp)) {

                        // This part of the code is really CPU expensive and takes a lot of time.
                        // Array processing in PHP sucks...
                        $i_max = $attrs['count'];
                        for ($i = 0; $i < $i_max; $i++) {
                            $j_max = $attrs[$attrs[$i]]['count'];
                            for ($j = 0; $j < $j_max; $j++) {
                                $mailaddress = $attrs[$attrs[$i]][$j];
                                if ($mailaddress !== NULL) {
                                    preg_match('/.*@'.$md.'/', $mailaddress, $matches);
                                    if ($matches[0]) {
                                        ++$counter;
                                    }
                                }
                            }
                            if ($counter == $counter_limit + 1) {
                                return (sprintf ("> %d", $counter_limit));
                            }
                        }
                    }
                }
                return ($counter);

            } elseif ($maildomain_count_usage_externalscript) {

                $command = array($BASE_DIR."/bin/maildomains_maildomain-usage-counter",
                                 "-H", $config->current['SERVER'],
                                 "-b", $base,
                                 "-D", $config->current['ADMINDN'],
                                 $md);

                if (is_executable($command[0])) {
                    $descriptor_spec = [
                        0 => ['pipe', 'r'], // stdin
                        1 => ['pipe', 'w'], // stdout
                        2 => ['pipe', 'w'], // stderr
                    ];
                    $fh= proc_open($command, $descriptor_spec, $pipes);
                    fwrite ($pipes[0], $config->current['ADMINPASSWORD']);
                    fclose($pipes[0]);

                    $stdout = stream_get_contents($pipes[1]);
                    $stderr = stream_get_contents($pipes[2]);
                    fclose($pipes[1]);
                    fclose($pipes[2]);

                    $exitCode = proc_close($fh);

                    if ($exitCode !== 0) {
                        DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, join(" ", $command), "Execution failed, exit code: ".$exitCode);
                        DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, join(" ", $command), "Error message: ".$stderr);
                        $message= msgPool::cmdexecfailed($err,join(" ", $command), "maildomains");
                        msg_dialog::display(_("Error"), $message, ERROR_DIALOG);
                        return(_("unknown"));
                    } else {
                        DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, join(" ", $command), "Result: ".$stdout);
                        return(trim($stdout));
                    }
                }
            }
        }
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
