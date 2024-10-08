<table style='width:100%;' summary="{t}Edit mail domain{/t}">
  <tr>
    <td style='width:100%;'>

      <h3>
        {t}Generic{/t}
      </h3>

      <table style='width:100%;' summary="{t}Generic settings{/t}">
        <tr>
          <td>{t}Mail domain{/t}{$must}</td>
          <td>
{render acl=$ouACL}
             <input type='text' value='{$ou}' name='ou'>
{/render}
          </td>
        </tr>
        <tr>
          <td>{t}Description{/t}</td>
          <td>
{render acl=$descriptionACL}
             <input type='text' value='{$description}' name='description'>
{/render}
          </td>
        </tr>
{if $base}
        <tr>
          <td>
            <div style="height:10px;"></div>
            <label for="base">{t}Base{/t}</label>
          </td>
          <td>
            <div style="height:10px;"></div>
{render acl=$baseACL}
            {$base}
{/render}
          </td>
        </tr>
{/if}
        <tr>
          <td colspan="2"><hr><br></td>
        </tr>

        <tr>
          <td>{t}Category{/t}</td>
          <td>
{render acl=$businessCategoryACL}
          <input type="radio" id="maildomainActive" name="businessCategory" value="ACTIVE" {$maildomainActive_checked} />
          <label for="maildomainActive">{t}Actively maintained email domain{/t}</label>
          <br />
          <input type="radio" id="maildomainRetired" name="businessCategory"  value="RETIRED" {$maildomainRetired_checked} />
          <label for="maildomainRetired">{t}Retired email domain{/t}</label>
          <br />
          <input type="checkbox" id="maildomainUnknown" name="businessCategoryUnknown" value="UNKNOWN" {$maildomainUnknown_checked} />
          <label for="maildomainUnknown">{t}Unknown in email system{/t}</label>
{/render}
          </td>
        </tr>

        <tr>
          <td colspan="2"><hr><br></td>
        </tr>

        <tr>
          <td>{t}Mail addresses used in this domain{/t}</td>
          <td>
{render acl=$countMaildomainUsageACL}
             <input type='text' value='{$countMaildomainUsage}' name='countMaildomainUsage'>
{/render}
          </td>
        </tr>

      </table>

    </td>
  </tr>
</table>
