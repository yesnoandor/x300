#!/bin/sh  
#-------------------------------------------------------------------------------------------------------------  
#实际使用中,需注意以下两点  
#1)直接在Linux/UNIX上面创建并编写shell脚本  
#  这是因为Windows下编写的*.sh文档格式和编码，是Linux/UNIX所不能完全承认的  
#  最直接的体现就是使用过程中会遇到很多似是而非的错误，这曾经搞疯了一大片人  
#2)编写完*.sh之后,记得[chmod 755 *.sh]，否则会提示无法执行or没有权限执行等等  
#-------------------------------------------------------------------------------------------------------------  
#JDK路径  
#JAVA_HOME="/usr/java/jdk1.6.0_31"  
  
#JVM启动参数  
#-server:一定要作为第一个参数,在多个CPU时性能佳  
#-Xloggc:记录GC日志,这里建议写成绝对路径,如此便可在任意目录下执行该shell脚本  
#JAVA_OPTS="-ms512m -mx512m -Xmn256m -Djava.awt.headless=true -XX:MaxPermSize=128m"  
JAVA_OPTS="-Duser.timezone=GMT+8 -server -Xms512m -Xmx512m -Xloggc:/app/code/CucPayTradePortalLog/gc.log"  
  
#Java程序日志所在的目录  
APP_LOG=/app/code/CucPayTradePortalLog  
  
#Java程序主体所在的目录,即classes的上一级目录  
APP_HOME=/app/code/CucPayTradePortal  
  
#Java主程序,即main(String[] args)方法类  
APP_MAIN=com.cucpay.tradeportal.MainApp  
  
#classpath参数,包括指定lib目录下的所有jar  
CLASSPATH=$APP_HOME/classes  
  
for tradePortalJar in "$APP_HOME"/lib/*.jar  
do  
   CLASSPATH="$CLASSPATH":"$tradePortalJar"  
done  
  
#-------------------------------------------------------------------------------------------------------------  
#getTradeProtalPID()-->获取Java应用的PID  
#说明:通过JDK自带的JPS命令及grep命令,准确查找Java应用的PID  
#其中:[jps -l]表示显示Java主程序的完整包路径  
#     awk命令可以分割出PID($1部分)及Java主程序名称($2部分)  
#例子:[$JAVA_HOME/bin/jps -l | grep $APP_MAIN]-->>[5775 com.cucpay.tradeportal.MainApp]  
#另外:用这个命令也可以直接取到程序的PID-->>[ps aux|grep java|grep $APP_MAIN|grep -v grep|awk '{print $2}']  
#-------------------------------------------------------------------------------------------------------------  
#初始化全局变量tradePortalPID,用于标识交易前置系统的PID,0表示未启动  
tradePortalPID=0  
   
getTradeProtalPID(){  
    javaps=`$JAVA_HOME/bin/jps -l | grep $APP_MAIN`  
    if [ -n "$javaps" ]; then  
        tradePortalPID=`echo $javaps | awk '{print $1}'`  
    else  
        tradePortalPID=0  
    fi  
}  
  
#-------------------------------------------------------------------------------------------------------------  
#startup()-->启动Java应用程序  
#步骤:1)调用getTradeProtalPID()函数,刷新$tradePortalPID全局变量  
#     2)若程序已经启动($tradePortalPID不等于0),则提示程序已启动  
#     3)若程序未被启动,则执行启动命令  
#     4)启动命令执行后,再次调用getTradeProtalPID()函数  
#     5)若步骤4执行后,程序的PID不等于0,则打印[Success],否则打印[Failed]  
#注意:[echo -n]表示打印字符后,不换行  
#注意:[nohup command > /path/nohup.log &]是将作业输出到nohup.log,否则它会输出到该脚本目录下的nohup.out中  
#-------------------------------------------------------------------------------------------------------------  
startup(){  
    getTradeProtalPID  
    echo "======================================================================================"  
    if [ $tradePortalPID -ne 0 ]; then  
        echo "$APP_MAIN already started(PID=$tradePortalPID)"  
        echo "======================================================================================"  
    else  
        echo -n "Starting $APP_MAIN"  
        nohup $JAVA_HOME/bin/java $JAVA_OPTS -classpath $CLASSPATH $APP_MAIN > $APP_LOG/nohup.log &  
        getTradeProtalPID  
        if [ $tradePortalPID -ne 0 ]; then  
            echo "(PID=$tradePortalPID)...[Success]"  
            echo "======================================================================================"  
        else  
            echo "[Failed]"  
            echo "======================================================================================"  
        fi  
    fi  
}  
  
#-------------------------------------------------------------------------------------------------------------  
#shutdown()-->停止Java应用程序  
#步骤:1)调用getTradeProtalPID()函数,刷新$tradePortalPID全局变量  
#     2)若程序已经启动($tradePortalPID不等于0),则开始执行停止程序操作,否则提示程序未运行  
#     3)使用[kill -9 PID]命令强制杀掉进程  
#     4)使用[$?]获取上一句命令的返回值,若其为0,表示程序已停止运行,则打印[Success],反之则打印[Failed]  
#     5)为防止Java程序被启动多次,这里增加了反复检查程序进程的功能,通过递归调用shutdown()函数的方式,反复kill  
#注意:Shell编程中,[$?]表示上一句命令或者上一个函数的返回值  
#-------------------------------------------------------------------------------------------------------------  
shutdown(){  
    getTradeProtalPID  
    echo "======================================================================================"  
    if [ $tradePortalPID -ne 0 ]; then  
        echo -n "Stopping $APP_MAIN(PID=$tradePortalPID)..."  
        kill -9 $tradePortalPID  
        if [ $? -eq 0 ]; then  
            echo "[Success]"  
            echo "======================================================================================"  
        else  
            echo "[Failed]"  
            echo "======================================================================================"  
        fi  
        getTradeProtalPID  
        if [ $tradePortalPID -ne 0 ]; then  
            shutdown  
        fi  
    else  
        echo "$APP_MAIN is not running"  
        echo "======================================================================================"  
    fi  
}  
  
#-------------------------------------------------------------------------------------------------------------  
#getServerStatus()-->检查程序运行状态  
#-------------------------------------------------------------------------------------------------------------  
getServerStatus(){  
    getTradeProtalPID  
    echo "======================================================================================"  
    if [ $tradePortalPID -ne 0 ]; then  
        echo "$APP_MAIN is running(PID=$tradePortalPID)"  
        echo "======================================================================================"  
    else  
        echo "$APP_MAIN is not running"  
        echo "======================================================================================"  
    fi  
}  
