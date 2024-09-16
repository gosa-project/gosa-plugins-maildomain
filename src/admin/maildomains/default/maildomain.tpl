<div class="role-wrapper">
  <div class="row">
    <div class="col s12 xl6">

      <h3>{t}Generic{/t}</h3>

      {render acl=$ouACL}
      <div class="input-field">
        <input type='text' value='{$ou}' name='ou' id="ou">
        <label for="ou">{t}Mail domain{/t}{$must}</label>
      </div>
      {/render}

      {render acl=$descriptionACL}
      <div class="input-field">
        <input type='text' value='{$description}' name='description' id="description">
        <label for="description">{t}Description{/t}</label>
      </div>
      {/render}

      {render acl=$baseACL}
      <div class="input-field ldap-tree">
        {$base}
      </div>
      {/render}

    </div>
  </div>
</div>
