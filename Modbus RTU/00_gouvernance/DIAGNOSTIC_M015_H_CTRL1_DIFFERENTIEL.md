# M015-H — Protocole différentiel CTRL1_XL

Initial main168ddbd444d0d8d6aa3ceabe115cc315d2ad5452, base6357b66 ; G DONE, premier refus10/A8→50. Acquis et campagnes A-G conservés, non rejoués. Nouveau harness strictement configuration, exclusif/OFF par défaut, SPI 3 mode 3/2,5 MHz calculé depuis HAL/prescaler64, CS PC9. Aucun DATA_OUT/FIFO_OUT, aucune écriture de contrôle FIFO, aucun média/UART/DMA/IRQ capteur.

## Variables et ordre fixés avant campagne

Préflight lecture seule : sept contrôles attendus0/0/0/0/0/4/0, audit des autres paramètres, WHO7B ; IF_INC présent. Trois registres voisins sont surveillés dans chaque lecture groupée. Les valeurs préexistantes ou un défaut de préflight interdisent les writes. DWT vérifié, HCLK160 MHz et SPI 2,5 MHz attendus.

| Phase | Modification avant lectures | Quatre lectures | Attendu CTRL1/CTRL3 |
|---|---|---|---|
| 1 baseline | aucune | S G G S | 00/04 |
| 2 préparé | CTRL3=44 uniquement | S G G S | 00/44 |
| 3 actif immédiat | unique write CTRL1=A8 | S G G S | A8/44 |
| 4 actif après délai | HAL_Delay20 ms, aucune autre write | S G G S | A8/44 |
| 5 arrêté | CTRL1=00 | S G G S | 00/44 |
| 6 restauration | CTRL1=00, CTRL3=04 | sept contrôles relus | initiaux |

S : TX90 00, deux octets, CTRL1 seul. G : TX8F puis six zéros, sept octets, WHO0F/CTRL1/byte11/CTRL3/CTRL4/CTRL5. Byte11 réservé enregistré sans attendu inventé. S G G S contrebalance l’ordre méthode/temps à l’intérieur de chaque phase, sans assurer simultanéité ; aucun vote ou fallback.

Le premier écart CTRL1 reste immuable et le résultat final reste en erreur, même si une lecture ultérieure est conforme. Seules les comparaisons en lecture prévues suivent un tel écart : le contrat H les autorise pour discrimination, et aucune acquisition n’est jamais admise. Un défaut HAL/compteurs/CS, WHO ou CTRL3/4/5, préflight ou NVIC arrête immédiatement vers restauration. À ce titre les contrôles d’acquisition F/G ne sont pas modifiés ni contournés.

Bornes : une activation,20 lectures comparatives maximum,43 transactions au nominal sous64traces allouées ; uniquement writes volatiles10=00/A8,12=04/44. Pas de boucle cherchant un succès, variante ou deuxième campagne. Les lectures de sauvegarde/restauration font partie de la sûreté. Les délais HAL ont la granularité SysTick et leur durée DWT effective sera rapportée ;20 ms est un choix de comparaison.

## Références et incertitudes

[ST DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), §§2.2 et9.11/9.12 : A8 active le capteur à ±4g ;50 ne correspond pas à l’encodage autorisé. IF_INC permet la lecture groupée ;44 conserve cet incrément et active BDU. Ton10 ms concerne l’obtention de données valides, pas un délai normatif de readback des registres. La comparaison20 ms ne présume aucun tel délai. Aucun débit/ODR ou signal au fil n’est mesuré. Les notes constructeur du dépôt et le HAL CubeU5v1.9.0 ont été relus ; le HAL bloquant lit les flags SPI et ne dépend pas d’une IRQ SPI. HAL_OK n’atteste pas la valeur des octets.

Le précédent M013-C/D est vérifié par ses preuves et DIAGNOSTIC_UART_IRQ_M013_D.md : HOTPLUG altérait NVIC, GDB seul puis reset qualifié rétablissait l’état. Aucun HOTPLUG ici. Nouveau profil sans IRQ externes attendues : ISER/ISPR0..2=0, masques0 ; SysTick est une exception système, distincte des IRQ49/66 nécessaires à M014 production. Captures CPU par phase (validités explicites) et SWD avant/après ; aucun registre NVIC réécrit. ICSR capturé sans imposer un état instantané de SysTick.

Ce protocole conserve BDU/IF_INC et la vitesse G, mais isole CTRL1 sans configurer le batching/STOP_ON_WTM de F/G. Buffers et instrumentation diffèrent ; une conclusion sur la méthode vaut pour cette campagne, pas pour toute configuration ni une cause électrique définitive.

## Validation et restauration

53 scénarios ciblés : nominal, dépendance méthode, deux méthodes incorrectes, anomalie transitoire persistée, divergence retardée, préexistants/WHO/CTRL3/NVIC/DWT, timeout à chacune des43 transactions nominales. Test ciblé1/1 et cross-build défaut/profil réussis. Profil concurrent rejeté, wrapper HP limité à deux lectures de reprise et syntaxe PS validée sans port. Cœur/V1 inchangés ; anciennes validations non répétées.

Préflight UID/image/STLINK et sauvegardes Flash2 Mio/RAM768 K avant flash. Une campagne, debugger détaché ; dumps avant restauration en finally. NORMAL08000000/verify/reset puis GDB seul ; pas mass erase/OptionBytes/RDP. M014-H doit retrouver Flash2 Mio byte-identique et contrôle minimal HP B0/B1 sans GDB pendant trafic. Les mesures BM869s ou substitution de breakout ne sont préparées que si les résultats le justifient ; aucune intervention physique H.

## Observation de l’unique campagne H

| Phase | S début | G premier | G second | S fin |
|---|---|---|---|---|
| Baseline | 00 | 00 | 00 | 00 |
| BDU préparé | 00 | 00 | 00 | 00 |
| Actif immédiat | A8 | A8 | A8 | **50** |
| Actif après20,27554 ms | A8 | A8 | A8 | **50** |
| Arrêté | 00 | 00 | 00 | 00 |

Premier écart trace 27, phase 3, TX90 00/RX00 50, expectedA8/observed 50 valides, HAL0/error0/counts0 ; DWT591951→594352 (15,00625 µs), CFG1=50070067/CFG2=87400000, SR A001→C007, GPIO CS1→0→1. WriteA8 trace 23, première S correcte trace 24 ; G 25/26 conformes. La même S fin est incorrecte trace 31 après délai, malgré S 28 et G 29/30 conformes. Diagnostic21,39172 ms,43 transactions/tousHAL_OK/deuxmismatches/restoration 0. Les traces groupées conservent WHO7B/CTRL3=44/CTRL4-5=0 stables, réservé11 simplement observé.

Conclusion exploitable : divergence associée à la position S finale dans la séquence active S G G S ; attendre20 ms n’élimine pas le défaut. Pas «toutes lectures simples fausses», pas preuve de writeA8 systématiquement raté ; pas état réel du registre établi indépendamment. L’ordre de transaction, son historique et la phase temporelle restent confondus ; cause précise nonattribuée. Au repos/arrêt aucun écart constaté. Le résultat reste9, gardeG nonrelâché, zéroacquisition.

Six snapshots CPU NVIC (phases 1..6/mask 126) ISER/ISPR0..2=0, masques0 ; SWD avant/après idem, faultregs0. SysTick/délaiHAL progressent. Ce profil n’active aucuneIRQ externe, donc ISER0 normal ; signature M013-D ISPRFFFFFFFF absente. Pas d’attribution au HOTPLUG (nonutilisé), ni exclusion de tout défaut transitoire par ces seuls snapshots.

M014-H restauré Flash2 Mio byte-identique ; sept contrôles0/0/0/0/0/4/0, gen12/IRQ49-66/configUNAVAILABLE/SDOFF et deux lecturesHP B0/B1 réussies. Journal complet et hashes publiés ; aucune deuxième campagne.

## Suite conditionnelle, sans intervention H

Priorité proposée : audit hors cible ciblé des transitions HAL entre transfert groupé septoctets et transfert simple deuxoctets, flags/clôture SPI/CS et contexte actif, à partir des traces exactes. Si un prochain essai est contracté, comparer une séquence S S S S et G S avec mêmes paramètres, bornes et attendus ; isoler historique/position avant toute correction. Cela est une proposition, pas un nouveau test H ni un fallback de production.

Pas de fiche BM869s proposée maintenant : un relevé DC ne discriminerait pas le motif de lecture observé ; aucune anomalie DC du rail capteur n’est mesurée ou présumée, aucune caractéristique du multimètre n’est inventée. Sa fréquence/MINMAX ne sont pas assimilées à une preuve d’octets SPI. Aucun achat d’oscilloscope nécessaire ou demandé.

Le second breakout identique est seulement déclaré dans le contrat. Une substitution A/B pourrait plus tard distinguer une dépendance au module/banc d’une dépendance commune côté MCU, si l’audit logiciel ne suffit pas. Protocole préparatoire futur : figer ce même code/mode/vitesse et la même séquence, identifier/photographier les modules et connecteurs réels ; utilisateur coupe l’alimentation avant toute substitution, conserve VDD/VDDIO3V3/GND/CS/MOSI/MISO/SCK et INTnonconnectées selon pinout confirmé ; aucune intervention sur broches fragiles. Sous contrat distinct, backups et restauration M014-H à chaque essai, aucun média ; comparer les bruts et non un verdict par vote. Une disparition avec B ne prouverait pas un capteur A défectueux, car la remise en place/câblage change aussi ; un retour contrôlé A serait nécessaire avant cette attribution. Aucune substitution, mesure DC ou action physique exécutée/demandée en H.
