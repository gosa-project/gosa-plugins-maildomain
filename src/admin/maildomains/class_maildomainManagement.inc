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

        // Add snapshot handler.
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }

        parent::__construct($config, $ui, "maildomains", $headpage);
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
