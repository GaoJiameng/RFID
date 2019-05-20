var flag1 = 0;//存放灯的状态 0关 1开
var flag2 = 0;//存放灯的状态 0关 1开
var flag3 = 0;//存放灯的状态 0关 1开

function deal_fun(arg) {
    switch (arg) {
        case 1:
            if(flag1 == 0)
            {
                flag1 = 1;
                document.getElementById("led1_img").src = "assets/img/images/led_on.png";
                document.getElementById("btn1").value = "off";
            }
            else{
                flag1 = 0;
                document.getElementById("led1_img").src = "assets/img/images/led_off.png";
                document.getElementById("btn1").value = "on";
            }
            // alert("led1");
            break;
        case 2:
            if(flag2 == 0)
            {
                flag2 = 1;
                document.getElementById("led2_img").src = "assets/img/images/led_on.png";
                document.getElementById("btn2").value = "off";
            }
            else{
                flag2 = 0;
                document.getElementById("led2_img").src = "assets/img/images/led_off.png";
                document.getElementById("btn2").value = "on";
            }
            break;
        case 3:
            if(flag3 == 0)
            {
                flag3 = 1;
                document.getElementById("led3_img").src = "assets/img/images/led_on.png";
                document.getElementById("btn3").value = "off";
            }
            else{
                flag3 = 0;
                document.getElementById("led3_img").src = "assets/img/images/led_off.png";
                document.getElementById("btn3").value = "on";
            }
            break;
        }
        //组一个发个服务器的包led1:1
        var data="led";
        data+=arg;
        data+=":";
        if(arg == 1)
            data+= flag1;
        else if(arg == 2)
            data+=flag2;
        else if(arg == 3)
            data+=flag3;

        send_data_to_server(data);

}

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

function send_data_to_server(data)
{
    // alert(data);

    //组url报文
    var url="/cgi-bin/led.cgi?";
    url+=data;
    // alert(url);

    //1、创建XMLHttpRequest对象
    var xmlRequest = null;
    xmlRequest = getXmlHttpRequest();

    //2、与回调函数关联起来
    xmlRequest.onreadystatechange=function()
    {
        //alert("----------");
        //alert("readyState = "+xmlRequest.readyState);
        //alert("status = "+xmlRequest.status);

        if(xmlRequest.readyState==4 && xmlRequest.status==200)
        {
            //用户要实现的地方
            //获取服务器的应答  而服务器应答存放在xmlRequest.responseText里面
            var text = xmlRequest.responseText;
            // alert(text);

        }
    }

    //3、使用open方法 创建请求
    xmlRequest.open("GET", url, true);
    xmlRequest.setRequestHeader("If-Modified-Since", "0");

    //4、发送请求
    xmlRequest.send();
}
