set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p/x &node
p/x &list
p sizeof(node)
p/x node
p/x list
p/x hsd2.Instance->IDMABAR
p/x hsd2.Instance->IDMALAR
p/x hsd2.Instance->IDMABASER
p/x hsd2.Instance->IDMABSIZE
p/x hsd2.Instance->STA
p/x hsd2.Instance->DCTRL

detach
quit
