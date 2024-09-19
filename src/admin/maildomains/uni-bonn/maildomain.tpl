<div class="container-fluid content-wrapper card-content-scroll">
  <div class="row content">
    <div class="col-sm-6">

      <h2>{t}Generic{/t}</h2>

      {render acl=$ouACL}
      <div class="row mb-1">
        <label for="cn" class="col-sm-3 col-form-label">{t}Mail domain{/t}{$must}</label>
        <div class="col-sm-9 input-field">
          <input type="text" id="ou" name="ou"  value="{$ou}" class="form-control" {if $is_new == "false"}disabled=""{/if}>
        </div>
      </div>
      {/render}

      {render acl=$descriptionACL}
      <div class="row mb-1">
        <label for="description" class="col-sm-3 col-form-label">{t}Description{/t}</label>
        <div class="col-sm-9 input-field">
          <input type="text" id="description" name="description"  value="{$description}" class="form-control">
        </div>
      </div>
      {/render}

      <div class="row mb-5">
      {if $base}
        <label for="use_base" class="col-sm-3 col-form-label">{t}Base{/t}{$must}</label>
        <div class="col-sm-9 input-field">
          {render acl=$baseACL}
          {$base}
          {/render}
        </div>
      </div>
      {/if}

      {render acl=$businessCategoryACL}
      <div class="row mb-1">
        <label class="col-sm-3 col-form-label">{t}Category{/t}</label>
        <div class="col-sm-9 form-check">
          <input class="form-radio-input" type="radio" id="maildomainActive" name="businessCategory" value="ACTIVE" {$maildomainActive_checked} />
          <label for="maildomainActive">{t}Actively maintained email domain{/t}</label>
          <br />
          <input class="form-radio-input" type="radio" id="maildomainRetired" name="businessCategory"  value="RETIRED" {$maildomainRetired_checked} />
          <label for="maildomainRetired">{t}Retired email domain{/t}</label>
        </div>
      </div>
      <div class="row mb-1">
        <div class="col-sm-3"></div>
        <div class="col-sm-9 form-check">
          <input class="form-radio-input" type="checkbox" id="maildomainUnknown" name="businessCategoryUnknown" value="UNKNOWN" {$maildomainUnknown_checked} />
          <label for="maildomainUnknown">{t}Unknown in email system{/t}</label>
        </div>
      </div>
      {/render}

    </div>

    {if $is_new == "false"}
    <div class="col-sm-6">

      <h2>{t}Administrative Information{/t}</h2>

      <div class="row mb-1">
        <label for="createTimestamp_" class="col-sm-4 col-form-label">{t}Creation date{/t}</label>
        <div class="col-sm-8 input-field">
          {render acl=$createTimestampACL}
            <input type="text" id="createTimestamp_" name="createTimestamp_" maxlength="60"
            value="{$createTimestamp|escape}" disabled="" class="form-control">
          {/render}
        </div>
      </div>

      <div class="row mb-1">
        <label for="creatorsName_" class="col-sm-4 col-form-label">{t}Created by{/t}</label>
        <div class="col-sm-8 input-field">
          {render acl=$creatorsNameACL}
            <input type="text" id="creatorsName_" name="creatorsName_" maxlength="60" value="{$creatorsName}" disabled="" class="form-control">
          {/render}
        </div>
      </div>

      <div class="row mb-1">
        <label for="modifyTimestamp_" class="col-sm-4 col-form-label">{t}Last modified{/t}</label>
        <div class="col-sm-8 input-field">
          {render acl=$modifyTimestampACL}
            <input type="text" id="modifyTimestamp_" name="modifyTimestamp_" maxlength="60"
            value="{$modifyTimestamp}" disabled="" class="form-control">
          {/render}
        </div>
      </div>

      <div class="row mb-1">
        <label for="modifiersName_" class="col-sm-4 col-form-label">{t}Last changed by{/t}</label>
        <div class="col-sm-8 input-field">
          {render acl=$modifiersNameACL}
            <input type="text" id="modifiersName_" name="modifiersName_" maxlength="60" value="{$modifiersName}" disabled="" class="form-control">
          {/render}
        </div>
      </div>

    </div>
    {/if}

  </div>
</div>
