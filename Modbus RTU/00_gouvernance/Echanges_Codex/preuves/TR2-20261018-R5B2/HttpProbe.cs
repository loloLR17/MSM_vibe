using System;using System.IO;using System.Net;using System.Diagnostics;using System.Threading;using System.IO.Ports;using System.Web.Script.Serialization;
class HttpProbe{
 static JavaScriptSerializer j=new JavaScriptSerializer();static void Log(object x){Console.WriteLine(j.Serialize(x));Console.Out.Flush();}
 static string Get(string path){var q=(HttpWebRequest)WebRequest.Create("http://127.0.0.1:5089"+path);q.Proxy=null;q.Timeout=2000;using(var r=(HttpWebResponse)q.GetResponse()){using(var s=new StreamReader(r.GetResponseStream())){string b=s.ReadToEnd();Log(new{kind="http",utc=DateTime.UtcNow.ToString("o"),path=path,status=(int)r.StatusCode,body=b});return b;}}}
 static int Main(string[] a){try{
 if(a.Length!=4||a[0]!="collect")throw new ArgumentException("collect seconds control-file toggle|steady");
 int seconds=int.Parse(a[1]);Stopwatch w=Stopwatch.StartNew();bool up=false,off=false,on=false;int n=0;
 while(w.Elapsed.TotalSeconds<seconds){
  if(a[3]=="toggle"&&w.Elapsed.TotalSeconds>=5&&!off){File.WriteAllText(a[2],"disconnected");off=true;Log(new{kind="operator_logical_disconnect",utc=DateTime.UtcNow.ToString("o")});}
  if(a[3]=="toggle"&&w.Elapsed.TotalSeconds>=8&&!on){File.Delete(a[2]);on=true;Log(new{kind="operator_logical_reconnect",utc=DateTime.UtcNow.ToString("o")});}
  try{if(!up){Get("/live.html");Get("/live.js");up=true;}Get("/api/v2/live");n++;}catch(WebException e){Log(new{kind="http_unavailable",utc=DateTime.UtcNow.ToString("o"),error=e.Message});}
  Thread.Sleep(250);
 }
 if(n==0)throw new Exception("No API response");Log(new{kind="collector_summary",passed=true,responses=n});return 0;
 }catch(Exception e){Log(new{kind="failure",error=e.ToString()});return 1;}}
}
