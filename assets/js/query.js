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



function on_button_query()
{
    var info = "";
    var query_method = document.getElementById("select_query").value;
    var text = document.getElementById("text_query").value;

    if("" == text)
    {
        alert("输入信息不能为空!");
        return;
    }
    else
    {
        info += query_method;
        info += ":";
        info += text;
    }

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

            var c = document.getElementById("myCanvas");

            var str=text.split("$");

            var x=10,y=40;
            document.getElementById("myCanvas").height = 200+str.length*y;
            var ctx=c.getContext("2d");
            ctx.font="18px Arial";
            for(a=0;a<str.length;a++)
            {
                ctx.fillText(str[a],10,y*(a+1));
            }

        }
    }

    var URL = "/cgi-bin/query.cgi";
    xmlRequest.open("POST",URL,true);
    xmlRequest.send(info);
    alert(info);


}