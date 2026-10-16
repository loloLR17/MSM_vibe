set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set $r4d_sensor=(Stm32Iis3dwbVibrationSource*)tr2_commission_fram_backup
set $r4d_id=(int)stm32_iis3dwb_vibration_source_init($r4d_sensor,&hspi3,(GPIO_TypeDef*)0x42020800,512)
p $r4d_id
if $r4d_id != 0
 quit 5
end
set $r4d_buffer=tr2_commission_fram_backup+128
p (int)read_registers($r4d_sensor,15,$r4d_buffer,1)
p (int)read_registers($r4d_sensor,16,$r4d_buffer+1,1)
p (int)read_registers($r4d_sensor,10,$r4d_buffer+2,1)
p (int)read_registers($r4d_sensor,25,$r4d_buffer+3,1)
x/4bx $r4d_buffer
if $r4d_buffer[0] != 0x7B || ($r4d_buffer[1]&0xE0) != 0 || ($r4d_buffer[2]&7) != 0 || ($r4d_buffer[3]&0x20) != 0
 quit 6
end

detach
quit
