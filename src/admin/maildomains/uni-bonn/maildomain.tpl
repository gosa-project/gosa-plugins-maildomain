<div class="container-fluid content-wrapper card-content-scroll">
  <div class="row content">
    <div class="col-sm-6">

      <h2>{t}Generic{/t}</h2>

      {render acl=$ouACL}
      <div class="row mb-1">
        <label for="cn" class="col-sm-3 col-form-label">{t}Mail domain{/t}{$must}</label>
        <div class="col-sm-9 input-field">
          <input type="text" id="ou" name="ou"  value="{$ou}" class="form-control">
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

      <div class="row mb-5 base-image">
        <label for="base" class="col-sm-3 col-form-label">{t}Base{/t}</label>
        <div class="col-sm-9 input-field">
          <div class="add one-line">
            {render acl=$baseACL}
              <select class="form-select" name="base" id="base" title="{t}Choose subtree to place mail domain in{/t}">
                {html_options options=$bases selected=$base_select}
              </select>
            {/render}

            {render acl=$baseACL disable_picture='images/lists/folder_grey.png'}
              <div class="file-image">
                <input type="image" id="chooseBase" name="chooseBase" src="images/lists/folder.png" title="{t}Select a base{/t}" class="form-control">
                <i class="bi bi-folder2-open"></i>
              </div>
            {/render}
          </div>
        </div>
      </div>

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

  </div>
</div>
