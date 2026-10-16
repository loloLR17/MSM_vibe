set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p/x ((RCC_TypeDef *)0x46020C00)->CSR
set var r4_result.ownership=0
set var live.geometry.physical_size=262144
set var live.geometry.superblock_a_base=0
set var live.geometry.superblock_b_base=4096
set var live.geometry.image_a_base=8192
set var live.geometry.image_b_base=65536
set var live.geometry.image_area_size=57344
p (int)transactional_image_media_init_v2(&historical,&bounded,&live.geometry,candidate,52298)
p (int)transactional_image_media_init_live(&live,&bounded,candidate,52298,0,ownership)
p (int)transactional_image_media_bind_work(&historical,&work)
p (int)transactional_image_media_bind_work(&live,&work)
set {unsigned int}engine.mailbox=0x004A003D
set {unsigned int}((char*)engine.mailbox+4)=0x38425018
set {unsigned int}((char*)engine.mailbox+8)=0x20333157
set {unsigned int}((char*)engine.mailbox+12)=1
p (int)live_configuration_init(&configuration,&historical,&live,uid,(PersistentIdentityAssignment*)engine.mailbox,1)
p (int)live_command_store_init(&store,&configuration)
p (int)tr2_cooperative_init(&work,0,work.now_us,work.service,work.safety_stop,32,1000,2500)
break cooperative_work.c:failed
commands
 silent
 bt
 p work
end
call live_configuration_recover(&configuration)
p work
p configuration.mounted

detach
quit
