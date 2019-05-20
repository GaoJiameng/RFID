function getXmlHttpRequest()
{
    var xmlRequest=null;
    if (window.XMLHttpRequest) {
        //如果是高版本 浏览器
        xmlRequest = new XMLHttpRequest();//创建对象
    } else {
        //低版本
        xmlRequest = new ActiveXObject("Microsoft.XMLHTTP");
    }

    return xmlRequest;
}


function on_button_add()
{
    var info = "";
    var name = document.getElementById("name").value;
    var id = document.getElementById("id").value;
    var sex = document.getElementById("sex").value;

    if("" == name || "" == id || "" == sex)
    {
        alert("输入信息不能为空!");
        return;
    }
    else
    {
        info += id;
        info += ":";
        info += name;
        info += ":";
        info += sex;
    }

    alert("请刷卡,然后点击确定!");

    //创建ajax对象
    var xmlRequest=null;
    xmlRequest = getXmlHttpRequest();

    //关联回调函数
    xmlRequest.onreadystatechange=function()
    {
        if(xmlRequest.readyState==4 && xmlRequest.status==200)
        {
            //用户要实现的地方
            //获取服务器的应答  而服务器应答存放在xmlRequest.responseText里面
            var text = xmlRequest.responseText;
            if(text == "succeed")
            {
                alert("注册成功");
            }
            else if(text == "failed")
            {
                alert("注册失败，此员工已注册");
            }

        }
    }


    var URL = "/cgi-bin/register.cgi";
    xmlRequest.open("POST",URL,true);
    xmlRequest.send(info);
    // alert(info);


}