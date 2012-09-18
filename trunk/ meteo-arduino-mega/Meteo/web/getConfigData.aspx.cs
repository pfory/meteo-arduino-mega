using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

public partial class getConfigData : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {
      String param=Request.QueryString.Get("param");
      if (param == "APIKEY")
        Data.Text = "HyVsT65CnEPitk6vML664llGUZCSAKx0aXFocmJJVHBUVT0g";
      else if (param == "STATIONNAME")
        Data.Text = "Solar";
      else if (param == "FEEDID")
        Data.Text = "75618";
      else if (param == "USERAGENT")
        Data.Text = "";
      else if (param == "high_above_sea")
        Data.Text = "34700";
      else if (param == "postingInterval")
        Data.Text = "20000";
      else if (param == "savinginterval")
        Data.Text = "20000";
      else if (param == "mac")
        Data.Text = "";
      else if (param == "server")
        Data.Text = "api.cosm.com";
      else if (param == "displayTempDelay")
        Data.Text = "1000";
      else if (param == "displayBMPDelay")
        Data.Text = "5000";
      else if (param == "displayDHTDelay")
        Data.Text = "4000";
      else if (param == "ALL")
        Data.Text = "HyVsT65CnEPitk6vML664llGUZCSAKx0aXFocmJJVHBUVT0g;Solar";
      else if (param == "CHECK")
        Data.Text = "0";
      else if (param == "COMMIT")
        Data.Text = "DONE";
    }
}