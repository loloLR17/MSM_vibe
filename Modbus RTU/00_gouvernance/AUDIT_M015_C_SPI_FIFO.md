# M015-C — Comparaison SPI/FIFO et diagnostic discriminant

## Lecture statique, 12 octobre 2026

[DS12569 Rev8 §§2.4 et3.2](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) : commande MSB-first, R/W=bit7, adresse7bits, un octet de commande puis les octets utiles sans octet dummy supplémentaire ; IF_INC en CTRL3_C. CS reste bas pour toute la transaction. Mode3 retenu, 625kHz inférieur au maximum10MHz ; setup/hold CS5/20ns restent des contraintes physiques, non mesurées par le code.

[AN5444 §6](https://www.st.com/resource/en/application_note/dm00683267-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) : BDU couvre les statuts ; ordre STATUS1→STATUS2 ; niveau10bits, overrun/latched et watermark distincts. FIFO_CTRL2 STOP_ON_WTM bit7 et WTM8 bit0 :0x81 est cohérent avec seuil256. Accès PDF direct expiré pendant C ; l’extrait ST indexé est consulté, l’audit B est conservé comme source antérieure explicitement identifiée.

[Driver officiel ST](https://github.com/STMicroelectronics/iis3dwb-pid/blob/master/iis3dwb_reg.c), copie consultée conservée dans les preuves B : `fifo_status_get` utilise un burst2 depuis3A ; `fifo_data_level_get` lit3A puis3B séparément. Le masque0x3F présent dans `fifo_status_get` de cette copie contredit la largeur10bits de la datasheet ; le harness B utilise0x03FF et n’est donc pas concerné par cette anomalie de copie ST. Pas d’import ni correction hors périmètre.

| Transaction | Commande TX puis zéros | Octets transférés | RX utile | CS |
|---|---|---:|---|---|
| WHO driver A et harness B |8F 00|2|rx[1]|bas→HAL duplex→haut|
| XYZ driver A |A8 +6zéros|7|rx[1..6]|identique|
| STATUS harness B |BA 00 00|3|rx[1..2], little endian &03FF|identique|
| FIFO_CTRL2 B |88 00|2|rx[1]|identique|
| FIFO word B (non atteint physiquement) |F8 +7zéros|8|rx[1..7]|identique|

Buffers B8octets contre A7, tailles bornées et copie après HAL_OK. Pas de consommation RXDR dans le code applicatif. Le HAL CubeU5 v1.9.0 écrit TXDR8bits, traite RXP puis RXWNE/RXPLVL pour les transferts courts, attend EOT et ferme le SPI en contrôlant les erreurs. Le seuil8DATA commun à A/B rend les longueurs2/3/7 pertinentes ; aucune faute HAL démontrée sans observation pendant transfert. Les SR avant/après ne capturent pas tous les états internes effacés par HAL.

## Hypothèses et contrôle minimal

| Hypothèse | Preuve actuelle | Discriminant borné / limite |
|---|---|---|
| Adresse, dummy, taille ou unpack faux | framing A/B conforme ; les171bruts sont inchangés | comparer WHO1/WHO2, contrôles single/burst et statuts single/pair dans un probe sans écriture |
| Erreur de lecture SPI indépendante du remplissage | motifs décalage/pertebit7 de B, non exhaustifs | répétitions de constantes WHO7B/CTRL3=04 avec RX bruts ; une incohérence constante exclut le mouvement et le compteur FIFO comme explication suffisante |
| BDU ou carry entre octets | premier127→0 avant carry255 ; cause seule insuffisante | ordre3A→3B conservé ; statut vide ne qualifie pas les transitions actives |
| Configuration FIFO réellement changée |81 puis80, lectures non indépendantes | FIFO_CTRL2 au repos attendu0 ; zéro stable ne démontre pas fiabilité de bit7/bit0 configurés |
| HAL/électrique/framing réel CS/SCK | HAL_OK ne certifie pas les octets | flags/compteurs/marqueurs latchCS puis, si nécessaire, capture logique SCK/MOSI/MISO/CS ; latch GPIO n’est pas mesure électrique |

## Probe C

Option OFF par défaut et exclusive ; branche avant production/médias/UART. TriggerRAM unique4D313543. Huit types de lecture×64 : WHO1/2, CTRL3, FIFO_CTRL2, burst6 depuis07, statuts pair puis singles. Pas lecture XYZ/FIFO_DATA_OUT, pas écriture capteur, pas acquisition ou watermark.512transactions et deadline1s ; octetcommande RX conservé sans l’interpréter comme donnée. DWT et CS sont marqueurs logiciels. STATUS2 peut effacer OVR_LATCHED ; trace conservée, aucun overrun volontaire.

Test nouveau uniquement :HAL simulé vérifie CS/commande read/absenceDATA_OUT/longueurs/octetcommandepréservé, timeout injecté et erreur brute conservée, arrêt512. Il ne valide ni HAL réel ni bus. Cross-build défaut et profil dédié réussis ; symboles médias/transport/production/captures absents du profil. Les tests A/B/.NET déjà acquis ne sont pas rejoués.

## Résultat de l’unique campagne C

512transactions terminées en128ms, mode3/625kHz/HCLK160MHz ;64répétitions de chacun des8types. WHO1 etWHO2 donnent7B, CTRL3 donne04 ; FIFO_CTRL2, bloc07..0C et statuts FIFO vides sont tous0. Aucune divergence utile, erreurHAL, transfert restant ou marqueurCS incohérent. L’octet RX de commande varie (00 ouFF) : non interprété comme donnée. Tableau attendu/observé et tous les RX/TX dans les preuvesC.

SR après HAL :0xC007(320fois),0xA007(128),0xA001(64). RXWNE/RXPLVL et parfoisRXP restent visibles après la fermeture SPI par HAL ; ces instantanés ne sont pas des traces des flags pendant transfert, et ne démontrent ni données résiduelles consommées ni cause de corruption. CFG1=70070067/CFG2=87400000 identiques aux paramètres B.

[Errata STM32U575 ES0499 Rev12 §2.23](https://www.st.com/resource/en/errata_sheet/es0499-stm32u575xx-and-stm32u585xx-device-errata-stmicroelectronics.pdf) : le cas de corruption du dernier RX pour CRCSIZE<7 avec CRC désactivé n’a pas sa condition CRCSIZE ici (valeur7 dansCFG1, également relevée enB). L’erratum de troncature des sorties après EOT est un point de contrôle temporel ; aucun front n’est mesuré et aucune attribution causale ni workaround non démontré n’est appliqué.

**BLOCKED cause M015-B / fiabilité FIFO active.** Le repos à constantes etFIFO vide est fiable dans cette observation bornée ; ce n’est pas la preuve des bits hauts du niveau ni deFIFO_CTRL2=81 en configuration active. BDU n’est pas activé (CTRL3=04), car ce probe n’écrit rien et n’a pas de conversions ; sa cohérence active reste non vérifiée. Aucune nouvelle capture ou correction heuristique. Aucune faute capteur/HAL/électrique attribuée.

## Protocole de mesure préparé, NON EXÉCUTÉ

Si la suite exige une preuve du framing électrique : utilisateur connecte un analyseur compatible3,3V, entrées haute impédance, au GND logique capteur et aux nets CS_PC9/SCK_PC10/MISO_PC11/MOSI_PC12 déjà documentés. Ne pas relier une sortie d’analyseur, une alimentation ou la partie isolée RS485 ; si références électriques ou instrument inconnus, suspendre ce branchement. Mesurer en mode3/MSB-first, fréquence d’échantillonnage adaptée au625kHz (au moins10MHz pour décodage, insuffisant pour certifier5/20ns).

Déclencher surCS descendant ; vérifier commande8F/88/BA,16/24clocks, CScontinu, absence d’octetdummy, fronts et niveau logique au premier/dernierbit. Comparer le décodage auRX brut du même identifiant de transaction. Ce contrôle peut distinguer byte déjà faux surMISO de byte correct surfil mais faux enRAM ; une capture sans anomalie ne démontre pas la causeB. HorodatageGPIO/DWT ne mesure pas setup/hold électrique. Aucune connexion instrument exécutée ; toute répétition ou campagne FIFO active nécessite un contrat distinct, le budgetC étant consommé.
