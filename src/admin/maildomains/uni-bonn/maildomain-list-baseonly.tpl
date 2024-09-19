<div class="list-head-wrapper">
  {if isset($SIZELIMIT) && $SIZELIMIT != ""}
    <div class="header-size-limit">
      <h2>{$HEADLINE}</h2>
      {$SIZELIMIT}
    </div>
  {else}
    <h2>{$HEADLINE}</h2>
  {/if}

  <input type="hidden" name="ignore">
  <div class="control-panel">
    <div class="navigation">
        {$ROOT}
        {$BACK}
        {$HOME}
        {$RELOAD}
    </div>

    {if isset($ACTIONS)}
      <div class="actions dropdown">{$ACTIONS}</div>
    {/if}
  </div>
</div>

<div class="table-list-container card-content-scroll">
  <div class="row">
    <div class="col-lg-8 col-xl-9">
      <div class="table-container">
        {$LIST}

        <input type="hidden" name="ignore">
      </div>
    </div>

    {if isset($FILTER)}
    <div class="col-lg-4 col-xl-3 filter-container">
      <div class="filter-wrapper">
        {$FILTER}
      </div>
    </div>
    {/if}

    {if isset($NOT_VISSIBLE_FILTER)}
    <div class="col-12 filter-hidden-wrapper">
      {$NOT_VISSIBLE_FILTER}
    </div>
    {/if}
  </div>

  {if isset($LIST_TABLE_SUMMARY)}
  <div class="row table-list-container">
    <div class="col-sm-12">
      {$LIST_TABLE_SUMMARY}
    </div>
  </div>
  {/if}
</div>
