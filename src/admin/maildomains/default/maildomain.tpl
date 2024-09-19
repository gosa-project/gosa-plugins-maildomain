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

      {if $base}
      {render acl=$baseACL}
      <div class="input-field ldap-tree">
        {$base}
      </div>
      {/render}
      {/if}

      {render acl=$countMaildomainUsageACL}
      <div class="input-field">
        <input type='text' value='{$countMaildomainUsage}' name='countMaildomainUsage' disabled="" id="description">
        <label for="countMaildomainUsage">{t}Mail addresses used in this domain{/t}</label>
      </div>
      {/render}

      {render acl=$businessCategoryACL}
      <div class="input-field">
        <label>{t}Category{/t}</label>
        <div>
          <input type="radio" id="maildomainActive" name="businessCategory" value="ACTIVE" {$maildomainActive_checked} />
          <label for="maildomainActive">{t}Actively maintained email domain{/t}</label>
          <br />
          <input type="radio" id="maildomainRetired" name="businessCategory"  value="RETIRED" {$maildomainRetired_checked} />
          <label for="maildomainRetired">{t}Retired email domain{/t}</label>
          <br />
          <input type="checkbox" id="maildomainUnknown" name="businessCategoryUnknown" value="UNKNOWN" {$maildomainUnknown_checked} />
          <label for="maildomainUnknown">{t}Unknown in email system{/t}</label>
        </div>
      </div>
      {/render}

    </div>
  </div>
</div>
