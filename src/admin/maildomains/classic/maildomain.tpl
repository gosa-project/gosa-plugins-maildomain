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
        <tr>
          <td colspan="2"><hr><br></td>
        </tr>
      </table>

    </td>
  </tr>
</table>
