<div class="filter-content">
  <div class="filter-head">
    <h3>Filter</h3>
    <i class="bi bi-funnel"></i>
  </div>

  <hr class="divider">

  <div class="filter card-content-scroll">
    <div class="form-check">
      {$ACTIVE}
      <label for="ACTIVE">{t}Maildomain active{/t}</label>
    </div>

    <div class="form-check">
      {$RETIRED}
      <label for="RETIRED">{t}Maildomain retired{/t}</label>
    </div>

    <div class="form-check">
      {$UNKNOWN}
      <label for="UNKNOWN">{t}Maildomain unknown to mailsystem{/t}</label>
    </div>

    <hr class="divider">

    <div class="row">
      <label for="MAILDOMAIN" class="col-sm-6 col-form-label">
        <i class="bi bi-search"></i>Mail Domain
      </label>

      <div class="col-sm-6 input-field">
        {$MAILDOMAIN}
      </div>
    </div>

    <hr class="divider">

    {$SCOPE}
  </div>
</div>

<div class="card-action">
  {$APPLY}
</div>
