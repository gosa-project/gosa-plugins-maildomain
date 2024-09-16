# Installation

## Install plugin in GOsa²

After installation of gosa-plugins-maildomain into the GOsa² source tree,
you need to adjust /etc/gosa/gosa.conf as shown by this patch:


```
diff --git a/contrib/gosa.conf b/contrib/gosa.conf
index 6e7e908b4..cda44c0b0 100644
--- a/contrib/gosa.conf
+++ b/contrib/gosa.conf
@@ -34,6 +34,7 @@
             class="userManagement" />
       <plugin acl="groups" class="groupManagement" />
       <plugin acl="roles" class="roleManagement"/>
+      <plugin acl="maildomain" class="maildomainManagement"/>
       <plugin acl="acl"  class="aclManagement" />
       <plugin acl="ogroups" class="ogroupManagement" />
       <plugin acl="sudo" class="sudoManagement" />
@@ -285,6 +286,10 @@
     <tab class="DynamicLdapGroup" name="Dynamic object" />
   </domain_tabs>
 
+  <maildomaintabs>
+    <tab class="maildomain" name="Generic"/>
+  </maildomaintabs>
+
   <!-- Role tabs -->
   <roletabs>
     <tab class="roleGeneric" name="Generic"/>
```

To adjust the storage point for mail domains, you can add
``maildomainRDN="ou=maildomains"`` to the ``&lt;ain />`` XML node of
``gosa.conf``.
