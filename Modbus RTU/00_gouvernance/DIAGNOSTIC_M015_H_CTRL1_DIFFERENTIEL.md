# M015-H — Protocole différentiel CTRL1_XL

Initial main168ddbd444d0d8d6aa3ceabe115cc315d2ad5452, base6357b66 ; G DONE, premier refus10/A8→50. Acquis et campagnes A-G conservés, non rejoués. Nouveau harness strictement configuration, exclusif/OFF par défaut, SPI3 mode3/2,5MHz calculé depuis HAL/prescaler64, CS PC9. Aucun DATA_OUT/FIFO_OUT, aucune écriture de contrôle FIFO, aucun média/UART/DMA/IRQ capteur.

## Variables et ordre fixés avant campagne

Préflight lecture seule : sept contrôles attendus0/0/0/0/0/4/0, audit des autres paramètres, WHO7B ; IF_INC présent. Trois registres voisins sont surveillés dans chaque lecture groupée. Les valeurs préexistantes ou un défaut de préflight interdisent les writes. DWT vérifié, HCLK160MHz et SPI2,5MHz attendus.

| Phase | Modification avant lectures | Quatre lectures | Attendu CTRL1/CTRL3 |
|---|---|---|---|
| 1 baseline | aucune | S G G S | 00/04 |
| 2 préparé | CTRL3=44 uniquement | S G G S | 00/44 |
| 3 actif immédiat | unique write CTRL1=A8 | S G G S | A8/44 |
| 4 actif après délai | HAL_Delay20ms, aucune autre write | S G G S | A8/44 |
| 5 arrêté | CTRL1=00 | S G G S | 00/44 |
| 6 restauration | CTRL1=00, CTRL3=04 | sept contrôles relus | initiaux |

S : TX90 00, deux octets, CTRL1 seul. G : TX8F puis six zéros, sept octets, WHO0F/CTRL1/byte11/CTRL3/CTRL4/CTRL5. Byte11 réservé enregistré sans attendu inventé. S G G S contrebalance l’ordre méthode/temps à l’intérieur de chaque phase, sans assurer simultanéité ; aucun vote ou fallback.

Le premier écart CTRL1 reste immuable et le résultat final reste en erreur, même si une lecture ultérieure est conforme. Seules les comparaisons en lecture prévues suivent un tel écart : le contrat H les autorise pour discrimination, et aucune acquisition n’est jamais admise. Un défaut HAL/compteurs/CS, WHO ou CTRL3/4/5, préflight ou NVIC arrête immédiatement vers restauration. À ce titre les contrôles d’acquisition F/G ne sont pas modifiés ni contournés.

Bornes : une activation,20 lectures comparatives maximum,43 transactions au nominal sous64traces allouées ; uniquement writes volatiles10=00/A8,12=04/44. Pas de boucle cherchant un succès, variante ou deuxième campagne. Les lectures de sauvegarde/restauration font partie de la sûreté. Les délais HAL ont la granularité SysTick et leur durée DWT effective sera rapportée ;20ms est un choix de comparaison.

## Références et incertitudes

[ST DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), §§2.2 et9.11/9.12 : A8 active le capteur à ±4g ;50 ne correspond pas à l’encodage autorisé. IF_INC permet la lecture groupée ;44 conserve cet incrément et active BDU. Ton10ms concerne l’obtention de données valides, pas un délai normatif de readback des registres. La comparaison20ms ne présume aucun tel délai. Aucun débit/ODR ou signal au fil n’est mesuré. Les notes constructeur du dépôt et le HAL CubeU5v1.9.0 ont été relus ; le HAL bloquant lit les flags SPI et ne dépend pas d’une IRQ SPI. HAL_OK n’atteste pas la valeur des octets.

Le précédent M013-C/D est vérifié par ses preuves et DIAGNOSTIC_UART_IRQ_M013_D.md : HOTPLUG altérait NVIC, GDB seul puis reset qualifié rétablissait l’état. Aucun HOTPLUG ici. Nouveau profil sans IRQ externes attendues : ISER/ISPR0..2=0, masques0 ; SysTick est une exception système, distincte des IRQ49/66 nécessaires à M014 production. Captures CPU par phase (validités explicites) et SWD avant/après ; aucun registre NVIC réécrit. ICSR capturé sans imposer un état instantané de SysTick.

Ce protocole conserve BDU/IF_INC et la vitesse G, mais isole CTRL1 sans configurer le batching/STOP_ON_WTM de F/G. Buffers et instrumentation diffèrent ; une conclusion sur la méthode vaut pour cette campagne, pas pour toute configuration ni une cause électrique définitive.

## Validation et restauration

53 scénarios ciblés : nominal, dépendance méthode, deux méthodes incorrectes, anomalie transitoire persistée, divergence retardée, préexistants/WHO/CTRL3/NVIC/DWT, timeout à chacune des43 transactions nominales. Test ciblé1/1 et cross-build défaut/profil réussis. Profil concurrent rejeté, wrapper HP limité à deux lectures de reprise et syntaxe PS validée sans port. Cœur/V1 inchangés ; anciennes validations non répétées.

Préflight UID/image/STLINK et sauvegardes Flash2Mio/RAM768K avant flash. Une campagne, debugger détaché ; dumps avant restauration en finally. NORMAL08000000/verify/reset puis GDB seul ; pas mass erase/OptionBytes/RDP. M014-H doit retrouver Flash2Mio byte-identique et contrôle minimal HP B0/B1 sans GDB pendant trafic. Les mesures BM869s ou substitution de breakout ne sont préparées que si les résultats le justifient ; aucune intervention physique H.
