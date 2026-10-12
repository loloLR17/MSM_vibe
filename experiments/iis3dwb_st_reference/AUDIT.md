# Audit ST / I / J — lecture de code, pas qualification matérielle

Sources upstream figées dans `vendor/SOURCES.json`, avec licences BSD-3-Clause
et SHA-256 de chaque fichier byte-identique. Exemples :
[STMems commit 1f848c38e84b4144d55c66fb5411e78bdb9f96f1](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/1f848c38e84b4144d55c66fb5411e78bdb9f96f1/iis3dwb_STdC/examples).
Driver : [iis3dwb-pid commit f83e74fb9945fc4dde4e058e7fcd2bdbded357b5](https://github.com/STMicroelectronics/iis3dwb-pid/tree/f83e74fb9945fc4dde4e058e7fcd2bdbded357b5).
Le gitlink du superprojet pointe `d995549f…` ; GitHub refuse sa récupération
(`not our ref`). Le driver réel est donc figé séparément au commit ci-dessus,
pas présenté comme une extraction récursive de ce gitlink indisponible.

Fichiers audités : exemples complets `iis3dwb_read_data_polling.c` et
`iis3dwb_fifo.c`, leurs APIs dans `iis3dwb_reg.c/.h`, callbacks dans `platform.c`,
portage `bench.c`, I `stm32_iis3dwb_burst_capture.c` et J
`stm32_iis3dwb_continuous.c` au HEAD initial `b8dad142…`.

| Étape / lignes ST d'origine | APIs / opération ST | I / J | Portage / conclusion statique |
|---|---|---|---|
| Polling L131..160, FIFO L137..162 | init callbacks, boot 10 ms, WHO7B, reset puis attente SW_RESET | Pas de reset capteur ; baseline puis restauration | Séquence ST conservée ; reset borné100ms, WHO erroné → aucune écriture |
| Polling L163..169, FIFO L165..168 | BDU puis ODR (polling), FS | I/J CTRL3=44, CTRL1=A8 ±4g | Polling FS2g/ODR puis filtre ; FIFO FS8g ; **différent** de I/J |
| Polling L171..174 | LP_ODR_DIV_100 | I/J pas ce filtre explicite | Conservé via API et relu ; bande utile différente, pas calibration |
| FIFO L170..180 | watermark256, batching26k7, STREAM6 | I STOP_ON_WTM256/FIFO1 puis XL arrêté ; J watermark128/STREAM6 | Ordre ST conservé, STOP_ON_WTM reset0 ; **I différent**, J mode conforme mais seuil différent |
| FIFO L183..185 | ODR puis DEC8 et timestamp enable | I/J pas timestamp | Conservé ; CTRL4 attendu86, CTRL10=20 après reset, CTRL1=AC ; J CTRL4=06/CTRL1=A8 |
| Driver auto_increment/SPI mode | IF_INC=1 au reset, SIM=0 | I/J explicit44 | Defaults vérifiés, pas écriture additionnelle ; conforme fonctionnellement, méthode différente |
| Callbacks ST platform_read/write | reg|80 lecture, commande TX puis RX, CS bas jusqu'à fin ; retour0 inconditionnel exemples | I/J HAL duplex, TX+RX897 ensemble | R/W, CS, IF_INC conformes ; TX/RX séparés ST conservés, erreurs propagées/timeout20ms/CS relevé même échec |
| Driver FIFO_STATUS get | burst 2 depuis3A, flags et niveau | I/J masque03FF/gardes propres | Défaut ST03F reproduit hors matériel ; aucun lien causal établi avec readback J |
| FIFO L192..200 et driver multi_raw | num entrées×7 depuis78 en une lecture | I/J 128×7 par burst F8 | Segmentation128 explicite pour SPI3≤1024, buffer512 borné ; original buffer256 peut déborder si num>256 |
| FIFO L211..227 | tag>>3, XL2/timestamp4, casts int16/int32 | I/J XL2 seulement/parité/counter | Conserve tous les7 octets, compte tags attendus, décodage offline little-endian sans cast désaligné |
| Exemples boucles infinies/tx_com | impression USB chaque donnée | I/J RAM bornée | Portage RAM/GDB,64XYZ ou512entrées/500ms, trigger unique et stop ; pas USB ni UART ajouté |
| Sortie | aucune fin normale dans ST | I/J sauvegarde/restauration7regs | Banc seul : arrêt XL/bypass/timestamp ; pas promesse de restaurer une config préexistante |

Les numéros correspondent aux copies figées ; une extraction
`nl -ba` est conservée dans les preuves avec les lignes complètes pour relecture.
Les lignes du portage correspondant à chaque API sont référencées dans cette
même preuve ; aucun code I/J n'est copié comme driver ou service.

Deux défauts upstream constatés dans le driver **inchangé** :

1. `iis3dwb_fifo_status_get`, L3276..3292, termine par `&0x3FU`.
   Mock brut `00 81` : watermark vrai et niveau256, API niveau0. ST_STRICT
   arrête avec `BENCH_ST_LEVEL`, sans accepter de données ni remplacer ce niveau.
   STATUS10 lit séparément les deux registres via l'API officielle `read_reg`,
   décide sur `&03FF` et conserve les deux observations. Ce sont deux transactions,
   pas un snapshot simultané ; les flags d'erreur de chacune sont contrôlés.
2. `iis3dwb_fifo_xl_batch_get`, L2871..2880, compare un champ4bits à `0x10`.
   Le setter écrit `0x0A`, qui donne NOT_BATCHED au getter. Le portage consigne
   `st_batch`, mais exige les bits bruts09=0A ; le mock reproduit les deux.
   Aucun patch vendeur ni substitution silencieuse ; le garde brut est commun
   aux profils et plus informatif que cet enum erroné.

Commentaire ST «12.5Hz» à FIFO batch est contredit par l'argument réel26k7 ;
le code/enum fait référence, pas ce commentaire. Les contrôles BDU/IF_INC/SIM,
FS/ODR/filtre, watermark/mode/timestamp, HAL/counts/CS, bornes et flags restent
actifs. Pas vote, correction des octets, retour succès artificiel ou profil TR2.
Parité et counters sont exportés pour analyse, pas qualifiés par ce programme.

Les anomalies I/J concernent des readbacks de configuration reçus ; ces deux
défauts de getter ne les expliquent pas. **Cause SPI/HAL/capteur non établie**.
Les transitions HAL commande puis Receive sous CS bas sont une différence
utile à essayer ensuite sur banc isolé ; aucune supériorité physique déduite
des tests mock. Il faudra figer paramètres et modules avant toute comparaison.
