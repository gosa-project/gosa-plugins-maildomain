<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 * Copyright (C) 2024 Mike Gabriel <mike.gabriel@das-netzwerkteam.de>
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

class maildomaintabs extends tabs
{
    var $base= "";
    var $acl_category;


    function __construct($config, $data, $dn, $category ="maildomains", $hide_refs = FALSE, $hide_acls = FALSE)
    {
        parent::__construct($config, $data, $dn, $category, $hide_refs, $hide_acls);
        $this->addSpecialTabs();
    }


    function save_object($save_current= FALSE)
    {
        tabs::save_object($save_current);

        /* Update reference, transfer variables */
        $baseobject= $this->by_object['maildomain'];
        foreach ($this->by_object as $name => $obj){

            /* Don't touch base object */
            if ($name != 'maildomain'){
                $obj->parent                    = &$this;
                $obj->ou                        = $baseobject->ou;
                $obj->assocaitedDomain          = $baseobject->associatedDomain;
                $obj->description               = $baseobject->description;
                $obj->businessCategory          = $baseobject->businessCategory;

                $this->by_object[$name] = $obj;
            }
        }
    }


    function save($ignore_account = FALSE)
    {
        global $config;
        $baseobject = $this->by_object['maildomain'];

        /* Check for new 'dn', in order to propagate the
         * 'dn' to all plugins
         */
        $ou      = preg_replace('/,/', '\,', $baseobject->ou); 
        $ou      = preg_replace('/"/', '\"', $ou); 
        $new_dn= 'ou='.$ou.','.get_ou("maildomain", "maildomainRDN").$baseobject->base;

        /* Move maildomain? */
        if (LDAP::fix($this->dn) != LDAP::fix($new_dn)){

            /* Write entry on new 'dn' */
            if ($this->dn != "new") {
                $baseobject->update_acls($this->dn, $new_dn);
                $baseobject->move($this->dn, $new_dn);
                $this->by_object['maildomain']= $baseobject;
            }

            /* Use the new DN from now on... */
            $this->dn = $new_dn;
        }

        if ($this->dn == "new"){
            $this->dn= 'ou='.$baseobject->ou.','.get_ou("maildomain", "maildomainRDN").$baseobject->base;
        }

        $ret = parent::save();
        return $ret;
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
