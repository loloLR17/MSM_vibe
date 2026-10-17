// R5AFIX bounded physical capture: ONE genuine START5/10 and STOP5/11 only.
// Windows SerialPort is exclusive; no retries on a transport/CRC error.
using System;using System.IO;using System.IO.Ports;using System.Diagnostics;using System.Threading;
using System.Collections.Generic;using System.Web.Script.Serialization;
class BenchMaster {
 static int startTx,stopTx;static bool refusal;static SerialPort port;static JavaScriptSerializer json=new JavaScriptSerializer();static int requests,valid,held;
 static void Log(object o){Console.WriteLine(json.Serialize(o));Console.Out.Flush();}
 static ushort Crc(byte[] data,int n){ushort crc=65535;for(int i=0;i<n;i++){crc^=data[i];for(int j=0;j<8;j++)crc=(ushort)((crc&1)!=0?(crc>>1)^0xA001:crc>>1);}return crc;}
 static byte[] Frame(byte[] pdu){byte[] q=new byte[pdu.Length+3];q[0]=1;Array.Copy(pdu,0,q,1,pdu.Length);ushort c=Crc(q,q.Length-2);q[q.Length-2]=(byte)c;q[q.Length-1]=(byte)(c>>8);return q;}
 static string Hex(byte[] b){return BitConverter.ToString(b).Replace("-"," ");}
 static byte[] Exchange(byte[] q){
  Thread.Sleep(25);if(port.BytesToRead!=0)throw new IOException("Unsolicited/stale bytes before request; no discard/retry.");
  Log(new{kind="request",utc=DateTime.UtcNow.ToString("o"),adu=Hex(q)});Stopwatch sw=Stopwatch.StartNew();port.Write(q,0,q.Length);List<byte> b=new List<byte>();
  b.Add((byte)port.ReadByte());b.Add((byte)port.ReadByte());int n;
  if((b[1]&128)!=0)n=5;
  else if(b[1]==3){b.Add((byte)port.ReadByte());n=5+b[2];}
  else if(b[1]==16)n=8;else throw new IOException("Unexpected function code");
  if(n>256)throw new IOException("Oversize response");while(b.Count<n)b.Add((byte)port.ReadByte());
  byte[] r=b.ToArray();bool crc=Crc(r,r.Length)==0;
  Log(new{kind="adu",utc=DateTime.UtcNow.ToString("o"),request=Hex(q),response=Hex(r),elapsed_ms=sw.Elapsed.TotalMilliseconds,crc_ok=crc});requests++;
  if(!crc||r[0]!=1||r[1]!=(q[1]))throw new IOException("CRC, unit or function/exception mismatch");return r;
 }
 static ushort[] Read(int a,int n){byte[] r=Exchange(Frame(new byte[]{3,(byte)(a>>8),(byte)a,(byte)(n>>8),(byte)n}));
  if(r.Length!=n*2+5||r[2]!=2*n)throw new IOException("FC03 length mismatch");ushort[] v=new ushort[n];
  for(int i=0;i<n;i++)v[i]=(ushort)(r[3+2*i]*256+r[4+2*i]);
  if(a==3000){
   if((v[1]&0xE100)!=0||v[3]>4||v[2]>511)throw new IOException("Reserved B3 flags/severity at normative offsets");
   int severity=(v[1]&6)!=6?0:(v[2]&15)!=0?4:(v[2]&240)!=0?3:(v[1]&64)==0?2:1;
   if(v[3]!=severity)throw new IOException("B3 3002 alarms / 3003 severity inconsistency");if((v[1]&64)!=0||v[4]!=0||v[5]!=0)throw new IOException("Invented UTC");if((v[1]&6)==6)valid++;if((v[1]&512)!=0)held++;}
  return v;
 }
 static void Write(int a,ushort[] v){byte[] p=new byte[6+2*v.Length];p[0]=16;p[1]=(byte)(a>>8);p[2]=(byte)a;p[4]=(byte)v.Length;p[5]=(byte)(2*v.Length);
  for(int i=0;i<v.Length;i++){p[6+2*i]=(byte)(v[i]>>8);p[7+2*i]=(byte)v[i];}byte[] r=Exchange(Frame(p));
  for(int i=0;i<4;i++)if(r[2+i]!=p[1+i])throw new IOException("FC10 echo mismatch");
 }
 static void Command(int tx,int code){Write(5020,new ushort[]{0,5});Write(5000,new ushort[]{(ushort)code,(ushort)tx,0,0,0,0,0,1});}
 static void Identity(){ushort[] b=Read(0,21);if(b[0]!=0||b[1]!=1||b[6]!=2||b[7]!=12)throw new IOException("Unexpected DeviceId/protocol/capabilities");Log(new{kind="identity",device=1,protocol=2,capabilities=b[7],subset_nonproduction=true});}
 static void Wait(int tx,int code,bool collision){Stopwatch w=Stopwatch.StartNew();while(w.Elapsed.TotalSeconds<210){
  ushort[] b=Read(5000,29);Read(3000,48);
  if(collision){if(b[10]==5&&b[11]==27){Log(new{kind="collision_refused",utc=DateTime.UtcNow.ToString("o"),tx=tx,result=27});return;}}
  else if(b[14]==code&&b[15]==tx&&b[16]==4&&b[17]==0&&(b[13]&2)==0&&b[10]==4){Log(new{kind="completed",utc=DateTime.UtcNow.ToString("o"),tx=tx,code=code,status=4,result=0,seconds=w.Elapsed.TotalSeconds});return;}
  if(b[10]==9||b[11]==28||b[11]==29)throw new IOException("Indeterminate/aborted transaction; no retry");Thread.Sleep(25);
 }throw new TimeoutException("R2 terminal not observed; no repeat");}
 static int Main(string[] args){try{
  if(args.Length==0||args[0]=="selftest"){
   byte[] known={1,3,3,232,0,1,4,122};if(Crc(known,known.Length)!=0)throw new Exception("CRC reference vector failed");
   Log(new{kind="selftest",passed=true,ports=SerialPort.GetPortNames(),no_transmission=true});return 0;
  }
  string mode=args[0];if(mode=="live"){if(args.Length!=2||args[1]!="fix")throw new ArgumentException("Explicit fix recipe required");refusal=false;startTx=10;stopTx=11;}if(mode!="ro"&&mode!="live")throw new ArgumentException("ro or live only");
  port=new SerialPort("COM4",115200,Parity.Even,8,StopBits.One);port.Handshake=Handshake.None;port.DtrEnable=false;port.RtsEnable=false;port.ReadTimeout=1500;port.WriteTimeout=1500;port.Open();
  Log(new{kind="opened",utc=DateTime.UtcNow.ToString("o"),port="COM4",baud=115200,format="8E1",mode=mode,exclusive=true,start_tx=startTx,stop_tx=stopTx,refusal_test=refusal});Identity();
  if(mode=="ro"){
   Stopwatch w=Stopwatch.StartNew();while(w.Elapsed.TotalSeconds<5){Read(3000,48);ushort[] b=Read(6000,64);if(b[0]!=2||b[4]!=0||b[9]!=4)throw new IOException("B6 unavailable mismatch");}
   if(valid!=0)throw new IOException("Diagnostic probe claimed authoritative V/F");
  }else{
   ushort[] initial=Read(3000,48);if(initial[0]!=1||(initial[1]&6)!=0)throw new IOException("Not idle before command");
   Command(startTx,13);Wait(startTx,13,false);if(valid==0)throw new IOException("No authoritative physical window");
   Stopwatch w=Stopwatch.StartNew();while(w.Elapsed.TotalSeconds<2)Read(3000,48);
   Command(stopTx,14);Wait(stopTx,14,false);
   ushort[] b=Read(3000,48);if((b[1]&4)!=0||(b[1]&512)==0||b[0]!=1)throw new IOException("STOP/HELD not observed");
  }
  Log(new{kind="summary",utc=DateTime.UtcNow.ToString("o"),mode=mode,requests=requests,valid_responses=valid,held_responses=held,passed=true});return 0;
 }catch(Exception e){Log(new{kind="failure",utc=DateTime.UtcNow.ToString("o"),type=e.GetType().FullName,error=e.Message,requests=requests,passed=false});return 1;}
 finally{if(port!=null){if(port.IsOpen)port.Close();port.Dispose();Log(new{kind="closed",utc=DateTime.UtcNow.ToString("o"),port="COM4"});}}
 }
}
