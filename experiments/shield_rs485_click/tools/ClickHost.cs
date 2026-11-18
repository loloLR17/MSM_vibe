// NON-PRODUCTION, bounded wire evidence. No service, media or persistent writes.
// Native Windows application; ordinary Windows application controls still apply.
using System;
using System.IO;
using System.IO.Ports;
using System.Diagnostics;
using System.Collections.Generic;
using System.Management;
using System.Threading;
using System.Web.Script.Serialization;

class ClickHost {
    static readonly JavaScriptSerializer Json = new JavaScriptSerializer();
    static void Log(object value) { Console.WriteLine(Json.Serialize(value)); Console.Out.Flush(); }
    static ushort Crc(byte[] data) {
        ushort c=65535;
        foreach(byte b in data) { c^=b; for(int j=0;j<8;j++) c=(ushort)((c&1)!=0?(c>>1)^0xA001:c>>1); }
        return c;
    }
    static byte[] Frame(byte[] body) {
        byte[] b=new byte[body.Length+2]; Array.Copy(body,b,body.Length);
        ushort c=Crc(body); b[b.Length-2]=(byte)c; b[b.Length-1]=(byte)(c>>8); return b;
    }
    static string Hex(byte[] b) { return BitConverter.ToString(b).Replace("-"," "); }
    static void Put32(byte[] b,int offset,uint value) {
        for(int j=0;j<4;j++) b[offset+j]=(byte)(value>>(8*j));
    }
    static string Adapter(string port) {
        string found=null;
        using(var query=new ManagementObjectSearcher("SELECT Name,PNPDeviceID FROM Win32_PnPEntity WHERE PNPClass='Ports'"))
        using(var devices=query.Get()) foreach(ManagementObject device in devices) {
            string name=Convert.ToString(device["Name"]),id=Convert.ToString(device["PNPDeviceID"]);
            if(name.EndsWith("("+port+")",StringComparison.OrdinalIgnoreCase)) {
                if(found!=null || !id.StartsWith(@"FTDIBUS\VID_0403+PID_6001+BG04KSSWA\",StringComparison.OrdinalIgnoreCase))
                    throw new IOException("Absent, ambiguous or unexpected adapter identity");
                found=id;
            }
        }
        if(found==null) throw new IOException("Expected adapter missing; no COM opened");
        return found;
    }
    static bool RawMatches(byte[] q,byte[] r) {
        if(r.Length!=26 || r[0]!=84 || r[1]!=49 || r[2]!=56 || r[3]!=33 || Crc(r)!=0) return false;
        for(int i=4;i<12;i++) if(q[i]!=r[i]) return false;
        byte[] uid=new byte[12]; Put32(uid,0,0x004A003D); Put32(uid,4,0x38425018); Put32(uid,8,0x20333157);
        for(int i=0;i<12;i++) if(uid[i]!=r[12+i]) return false;
        return true;
    }
    static void Selftest() {
        byte[] q=Frame(new byte[]{1,3,3,232,0,1}),r=Frame(new byte[]{1,3,2,0,0});
        if(Hex(q)!="01 03 03 E8 00 01 04 7A" || Hex(r)!="01 03 02 00 00 B8 44" || Crc(q)!=0 || Crc(r)!=0)
            throw new Exception("Known independent Modbus wire oracle failed");
        byte[] body=new byte[12]; Array.Copy(new byte[]{72,49,56,63},body,4); Put32(body,4,1934358945); Put32(body,8,7);
        byte[] raw=Frame(body),reply=new byte[24]; Array.Copy(new byte[]{84,49,56,33},reply,4);
        Array.Copy(body,4,reply,4,8); Put32(reply,12,0x004A003D); Put32(reply,16,0x38425018); Put32(reply,20,0x20333157);
        if(!RawMatches(raw,Frame(reply)) || RawMatches(raw,raw)) throw new Exception("Raw response/echo oracle failed");
        reply[4]^=1; if(RawMatches(raw,Frame(reply))) throw new Exception("Nonce mismatch accepted");
        reply[4]^=1; reply[12]^=1; if(RawMatches(raw,Frame(reply))) throw new Exception("Wrong UID accepted");
        byte[] bad=Frame(body); bad[13]^=1; if(Crc(bad)==0) throw new Exception("Corrupt CRC accepted");
        Log(new { kind="selftest",passed=true,no_COM_open=true,request=Hex(q),response=Hex(r),raw=Hex(raw),checks="CRC, known vectors, target UID, nonce, echo rejection" });
    }
    static int Main(string[] args) {
        SerialPort serial=null;
        try {
            if(args.Length==1 && args[0]=="selftest") { Selftest(); return 0; }
            if(args.Length!=5 || args[4]!="PHYSICAL-GATE-CONFIRMED" || (args[0]!="RX" && args[0]!="RAW" && args[0]!="MODBUS"))
                throw new ArgumentException("Explicit RX/RAW/MODBUS, COM, nonce and physical gate required");
            string mode=args[0],port=args[1],id=Adapter(port); uint nonce=UInt32.Parse(args[2]);
            int count=Int32.Parse(args[3]);
            // Fifth argument is the physical gate, fourth is bounded request count.
            if(count<1 || count>8 || (mode=="RX" && count!=1)) throw new ArgumentException("Request count out of bounds");
            serial=new SerialPort(port,115200,Parity.Even,8,StopBits.One);
            serial.Handshake=Handshake.None; serial.DtrEnable=false; serial.RtsEnable=false; serial.ReadTimeout=50; serial.WriteTimeout=1000;
            serial.Open(); // exclusive; never stop another owner or discard existing evidence
            Log(new {kind="opened",utc=DateTime.UtcNow.ToString("o"),mode=mode,port=port,adapter=id,baud=115200,format="8E1",exclusive=true});
            if(serial.BytesToRead!=0) throw new IOException("Unsolicited pending bytes; no discard/retry");
            for(int seq=0;seq<count;seq++) {
                byte[] body;
                if(mode=="MODBUS") body=new byte[]{1,3,3,232,0,1};
                else { body=new byte[12]; Array.Copy(new byte[]{72,49,56,63},body,4); Put32(body,4,nonce); Put32(body,8,(uint)seq); }
                byte[] q=Frame(body); var received=new List<byte>();
                if(serial.BytesToRead!=0) throw new IOException("Unexpected bytes between exchanges; no discard/retry");
                Log(new {kind="request",utc=DateTime.UtcNow.ToString("o"),sequence=seq,request_hex=Hex(q)});
                Stopwatch timer=Stopwatch.StartNew(); long first=-1; serial.Write(q,0,q.Length);
                while(timer.ElapsedMilliseconds<500) {
                    while(serial.BytesToRead!=0) {
                        if(first<0) first=timer.ElapsedTicks;
                        received.Add((byte)serial.ReadByte());
                        if(received.Count>1024) throw new IOException("Unexpected receive flood");
                    }
                    Thread.Sleep(1);
                }
                byte[] response=received.ToArray(); bool ok=mode=="RX"?response.Length==0:mode=="RAW"?RawMatches(q,response):Hex(response)=="01 03 02 00 00 B8 44";
                Log(new {kind="response",utc=DateTime.UtcNow.ToString("o"),sequence=seq,request_hex=Hex(q),response_hex=Hex(response),first_rx_ticks=first,qpc_hz=Stopwatch.Frequency,elapsed_ticks=timer.ElapsedTicks,host_match=ok,target_RAM_comparison_required=true});
                if(!ok) throw new IOException("Wire response mismatch; stop, no retry");
                Thread.Sleep(100);
            }
            Log(new {kind="summary",host_passed=true,target_proof_required=true,requests=count}); return 0;
        } catch(Exception e) {
            Log(new {kind="failure",utc=DateTime.UtcNow.ToString("o"),type=e.GetType().FullName,message=e.Message,host_passed=false}); return 1;
        } finally {
            if(serial!=null) { if(serial.IsOpen) serial.Close(); serial.Dispose(); Log(new {kind="closed",utc=DateTime.UtcNow.ToString("o")}); }
        }
    }
}
