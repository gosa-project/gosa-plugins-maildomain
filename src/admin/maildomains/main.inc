<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 * Copyright (C) 2024 Mike Gabriel <mike.gabriel@das-netzwerkteam.de>
 *
 * ID: $$Id: main.inc 9275 2008-03-04 07:29:22Z hickert $$
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

// Remove locks created by this plugin
if ($remove_lock) {
    if (session::is_set('maildomainManagement')) {
        $macl = session::get('maildomainManagement');
        $macl->remove_lock();
    }
}

// Remove this plugin from session
if ($cleanup) {
    session::un_set('maildomainManagement');
} else {
    // Create maildomainManagement object on demand
    if (!session::is_set('maildomainManagement')) {
        session::set('CurrentMainBase', $config->current['BASE']);
        session::set('maildomainManagement', new maildomainManagement($config, $ui));
    }
    $maildomainManagement = session::get('maildomainManagement');
    $output = $maildomainManagement->execute();

    // Page header
    if (get_object_info() != "") {
        $display = print_header(
            "bi-envelope-at",
            _("Mail Domain Management"),
            "<i class='bi bi-lock'></i><p class='info'>" .
                LDAP::fix(get_object_info()) . "</p>"
        );
    } else {
        $display = print_header(
            "bi-envelope-at",
            _("Mail Domain Management")
        );
    }

    $display .= $output;

    // Reset requested?
    if (isset($_GET['reset']) && $_GET['reset'] == 1) {
        session::un_set('maildomainManagement');
    }

    // Show and save dialog
    session::set('maildomainManagement', $maildomainManagement);
}
