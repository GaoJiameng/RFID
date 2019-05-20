function createXHR()
{
    var xmlhttp = null;
    if(window.XMLHttpRequest){
        xmlhttp = new XMLHttpRequest();
    }else if(window.ActiveXObject){
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }else{
        xmlhttp = new ActiveXObject('Msxml2.XMLHTTP');
    }
    if(xmlhttp){

    }else{
        alert('create xmlhttp error!');
    }
    return xmlhttp;
}

function get_add_info()
{
    var info = "1";
    var name = document.getElementById("name").value;
    var id = document.getElementById("id").value;
    var sex = document.getElementById("sex").value;

    if("" == name || "" == id || "" == sex)
    {
        alert("输入信息不能为空!");
        return info;
    }
    else
    {
        info += ":";
        info += name;
        info += ":";
        info += id;
        info += ":";
        info += sex;
    }
    alert(info);

    return info;
}

function get_del_info()
{
    var info = "2";
    var rfid_del = document.getElementById("rfid_del").value.toLocaleLowerCase();

    if("" == rfid_del)
    {
        alert("输入信息不能为空!");
        return info;
    }
    else
    {
        info += ":";
        info += rfid_del;
    }

    return info;
}


function get_query_info()
{
    var info = "3";
    var query_method = document.getElementById("select_query").value;
    var text = document.getElementById("text_query").value;

    if("" == text)
    {
        alert("输入信息不能为空!");
        return info;
    }
    else
    {
        info += ":";
        info += query_method;
        info += ":";
        info += text;
    }

    return info;
}



function get_cgi_result(urlcmd, type)//0:text;1:xml
{
    var xml = createXHR();
    if(xml)
    {
        xml.open("GET",urlcmd,false);//false:同步;true:异步
        xml.setRequestHeader("If-Modified-Since", "0");
        xml.send();
        if(200 == xml.status)
        {
            if(4 == xml.readyState)
            {
                if(0 == type)
                {
                    return xml.responseText;
                }
                else if(1 == type)
                {
                    return xml.responseXML;
                }
            }
        }
    }
    else
    {
        alert('please check the Bowser!');
    }



}

function deal_query_result(data_tmp)
{
    //clear

    var txt="<table id='customers'>";
    txt += "<tr>";
    txt += "<th>编号</th>";
    txt += "<th>姓名</th>";
    txt += "<th>性别</th>";
    txt += "<th>卡号</th>";
    txt += "<th>打卡时间</th>";
    txt += "</tr>";

    var x=data_tmp.documentElement.getElementsByTagName("INFO");
    for (i=0;i<x.length;i++)
    {

        if(i % 2 == 0)
        {
            txt=txt + "<tr>";
        }
        else
        {
            txt=txt + "<tr class='alt'>";
        }

        xx=x[i].getElementsByTagName("id");
        try
        {
            txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
        }
        catch (er)
        {
            txt=txt + "<td> </td>";
        }

        xx=x[i].getElementsByTagName("name");

        try
        {
            txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
        }
        catch (er)
        {
            txt=txt + "<td> </td>";
        }

        xx=x[i].getElementsByTagName("sex");

        try
        {
            txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
        }
        catch (er)
        {
            txt=txt + "<td> </td>";
        }


        xx=x[i].getElementsByTagName("rfid");

        try
        {
            txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
        }
        catch (er)
        {
            txt=txt + "<td> </td>";
        }

        xx=x[i].getElementsByTagName("time");

        try
        {
            txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
        }
        catch (er)
        {
            txt=txt + "<td> </td>";
        }

        txt=txt + "</tr>";
    }

    txt=txt + "</table>";
    document.getElementById('copy').innerHTML=txt;


}


function js_communicate_with_cgi(urlcmd, type)//0:text;1:xml
{
    //type:result:other
    var data_tmp = get_cgi_result(urlcmd, type);

    if(0 == type)//text
    {
        alert(data_tmp);
        var data = data_tmp.split(":");
        var act = parseInt(data[0]);
        var result = parseInt(data[1]);
        var other = data[2];

        switch(act)
        {
            case 1:
            {
                if(-1 == result)
                {
                    alert("卡号:"+other+"注册失败");
                }
                else if(0 == result)
                {
                    alert("卡号:"+other+"注册成功");
                }

            }
                break;

            case 2:
            {
                if(-1 == result)
                {
                    alert("卡号:"+other+"注销失败");
                }
                else if(0 == result)
                {
                    alert("卡号:"+other+"注销成功");
                }
            }
                break;


            default:
                break;


        }

    }
    else//xml
    {
        deal_query_result(data_tmp);
    }

}


function on_button_add()
{
    var cmd = get_add_info();
    if(cmd == "1")
        return;

    alert("点击确定后请刷卡!");
    var cmd1 = encodeURI(cmd);
    alert(cmd1);
    var URL = "/cgi-bin/deal_cmd.cgi?";
    URL += cmd1;
    js_communicate_with_cgi(URL, 0);


}

function on_button_del()
{
    var cmd = get_del_info();
    if(cmd == "2")
        return;

    alert(cmd);
    var URL = "/cgi-bin/deal_cmd.cgi?";
    URL += cmd;
    js_communicate_with_cgi(URL, 0);
}


function control_rfid_thread(on)
{
    var cmd = "100:";
    cmd += on;

    var URL = "/cgi-bin/deal_cmd.cgi?";
    URL += cmd;

    get_cgi_result(URL, 0);

}


function on_add_load()
{
    control_rfid_thread(0);
}


function on_add_beforeunload()
{
    control_rfid_thread(1);
}


function on_button_query()
{
    var cmd = get_query_info();
    if(cmd == "3")
        return;

//	alert(cmd);
    var cmd1 = encodeURI(cmd);

    var URL = "/cgi-bin/deal_cmd.cgi?";
    URL += cmd1;
    js_communicate_with_cgi(URL, 1);

}

