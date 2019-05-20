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

function my_login() {
    // body...
    var usr = document.getElementById('usr').value;
    var pwd = document.getElementById('pwd').value;

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

            if(text == "ok")
            {
                window.location.href="./admin.html";
            }
            else if(text == "no")
            {
                alert("账号或密码错误，请重新输入");
                document.getElementById('usr').value="";
                document.getElementById('pwd').value="";
            }


        }
    }

    //open
    var url="cgi-bin/login.cgi";
    var data=usr+":"+pwd;
    // alert(data);

    xmlRequest.open("POST",url,true);
    xmlRequest.send(data);
}