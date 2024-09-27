<div class="container-fluid content-wrapper card-content-scroll">
  <div class="row attention">
    <div class="col-sm-2 attention-icon">
      {image path='<i class="bi bi-exclamation-triangle"></i>'}
    </div>

    <div class="col-sm-10 attention-content">
      <h2>{t}Attention{/t}</h2>

      {if count($dns) == 1}
      {t}This object and its subobjects will be deleted:{/t}<br>
      {else}
      {t}These objects and their sub-objects will be deleted:{/t}
      {/if}
      <br />

      <ul>
      {foreach $dns as $dn}
      <li>{$dn}</li>
        {if count($dns_subobjects[$dn]) > 0}
        <ul>
          {foreach from=$dns_subobjects[$dn] item=dns_subobj}
          <li>{$dns_subobj}</li>
          {/foreach}
        </ul>
        {/if}
      {/foreach}
      </ul>

      <label>
        <span>{t}If you're sure you want to do this press 'Delete' to continue or 'Cancel' to abort.{/t}</span>
      </label>
    </div>
  </div>
</div>

<div class="card-action">
  <button class="btn-small primary" type='submit' name='delete_confirmed'>{$msgPool->delButton()}</button>
  <button class="btn-small primary" type='submit' name='delete_cancel'>{$msgPool->cancelButton()}</button>
</div>
