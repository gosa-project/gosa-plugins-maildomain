<div class=plugin>
  <div class='inline-warning'>
    {image path="images/warning.png" align="top"}
    <div class='inline-warning-text'>{t}Attention{/t}</div>
  </div>

  <hr>

  <div class='object-list-container'>
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
  </div>

  <p>
    {t}If you're sure you want to do this press 'Delete' to continue or 'Cancel' to abort.{/t}
  </p>

  <hr>

  <div class="plugin-actions">
    <button type='submit' name='delete_confirmed'>{msgPool type=delButton}</button>
    <button type='submit' name='delete_cancel'>{msgPool type=cancelButton}</button>
  </div>
</div>
