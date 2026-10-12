---
mission_id: TR2-20261013-M015-S-SPI-FREQUENCY-SWEEP
status: BLOCKED
base_ref: main
base_sha: 4c0785bf11dfedabe93e996fefecdb58971d0177
initial_head: 7e28d919974f7c24356b88c3ab72f1f5803fb513
result_sha: f7abff2b6f917b691d0564799bca19a5ee1f7bba
created_at_utc: 2026-10-12T23:19:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-S — échec SPI à 4 MHz ; arrêt de la campagne

L'unique essai 4096 à 4 MHz échoue au premier burst FIFO. Le récepteur SPI STM32 présente un overrun et le transfert se termine en timeout. **Aucun XYZ n'est admis. Le cleanup réussit. L'image 8 MHz est compilée mais n'a jamais été flashée ni exécutée**, conformément à l'arrêt obligatoire sur défaut SPI.

M015-S reste **BLOCKED pour la qualification 4096**. Les acquis de répétabilité et de 512 XYZ de Q sont conservés. Aucune fréquence ne peut être recommandée comme qualifiée pour 4096 avec le harness actuel.

## Résultat et preuves du premier refus

Trace32, indice zéro : HAL_TIMEOUT=3, erreur `0x104` = OVR `0x04` + TIMEOUT `0x100`. Le résultat cible est stage6/result1(BENCH_IO)/cleanup0. Juste avant le burst : niveau FIFO149, flags0. Le statut FIFO après l'échec n'est pas lu ; aucune absence d'overflow pendant le timeout n'est affirmée.

Le snapshot **avant fermeture HAL** montre :896 octets demandés, RXsize896, RXoffset63, RXremaining833, CS bas, SPI_SR=`0x104A` avec OVR, CFG1=`0x10070007` et CFG2=`0x87440000`. La phase payload dure19,82025ms jusqu'au timeout ; ce n'est pas une mesure de débit valide. Le champ error_before_close vaut0 malgré le bit OVR brut. La fermeture HAL remet ensuite les compteurs restants à0 : ces compteurs finaux ne prouvent pas un transfert complet.

Les896 octets du buffer demandé sont conservés comme **REJETÉS**. Seuls63 octets sont constatés consommés par l'offset ; le reste préinitialisé ne constitue pas des données acquises. Aucun segment128 ni préfixe XYZ qualifié n'est revendiqué. Parité, TAG_CNT et cohérence4096 restent NON VÉRIFIÉS faute de données admises. La comparaison des buffers admis est vide=vide, explicitement annotée ; elle ne qualifie pas le transfert rejeté.

## Comparaison des fréquences

| SCK calculé | CPU/diviseur | Payload | Cycle complet et marge | Statut4096 |
|---|---|---|---|---|
|2MHz, historique Q/R|16MHz/8|3,631625ms pour896 octets|4,988434ms ; déficit0,721768ms sur budget4,266667ms|BLOCKED overflow FIFO après2275XYZ ;512Q acquis|
|4MHz, unique essai S|16MHz/4|63 octets consommés, puis timeout19,82025ms|NON MESURABLE : aucun segment complet|BLOCKED SPI_OVR/TIMEOUT ;0XYZ admis|
|8MHz, image préparée|16MHz/2|NON TESTÉ|NON TESTÉ|NON TESTÉ après arrêt critique4MHz|

Le besoin nominal reste210000octets/s avec un timestamp pour8XYZ. Les mesures2MHz de Q/R sont réutilisées, sans répétition. La limite théorique500000octets/s à4MHz ne démontre pas un débit reçu fiable. Augmenter seulement la fréquence ne suffit pas avec ce service RX ; une responsabilité exclusive du CPU, du câblage ou du capteur n'est pas démontrée. La RCA historique nappe n'est pas réouverte.

## Préparation, code et validations

Le [protocole S et l'audit FRAM](../../../experiments/iis3dwb_st_reference/QUALIFICATION_M015_S.md) ont été fixés avant la cible. La [datasheet IIS3DWB Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), table5, donne SCKmax10MHz ;4 et8MHz restent sous cette limite documentaire. Le [schéma du kit ST](https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf) a été consulté. Rails3,3V, GND et câblage PC9..12 proviennent des confirmations utilisateur précédentes. Longueur, capacité et fronts du câblage ne sont pas mesurés ; aucune qualification électrique générale n'est revendiquée. Aucun geste physique n'a été demandé.

Seuls CMake/BENCH_SPI_DIVIDER et la sélection du prescaler dans src/platform.c changent :8 par défaut,4 ou2. CPU/PCLK3 MSI16, mode3, configuration/ODR ST, vendor, parser, gardes, réserves et instrumentation Q/R restent inchangés. Trois builds ARM réussissent en une passe ; le BIN2MHz est byte-identique au BIN Q4096. CMake refuse le diviseur16 comme attendu. Warnings newlib usuels conservés. Validation proportionnée à ce changement du harness : compilation et contrôle de configuration ; aucune suite hôte/cœur/.NET ni ancienne validation réussie rejouée.

Code réellement évalué : `f7abff2b6f917b691d0564799bca19a5ee1f7bba`, publié et32 fichiers intégralement relus avant matériel. BIN4 :55636octets/SHA256 `d836824452672f61819c900330408ba8aab9122c241cde1e1bf2eacd6d1253b1` ; ELF `fad9a85dd18d2ca48ad1223fb26ab6c6d324ddf978c0422e107ee7cc6456bf05`. BIN8 non flashé : `1c91bf71ab772a8ebd1c8615effc60046b21cc7f44cad25e4c5e529ef8f9a6a9`. Toutes les empreintes figurent dans artifacts.json.

L'adaptation initiale de l'analyseur Q comparait896 octets rejetés à0 octet admis et a refusé par assertion. Cette erreur hors ligne est conservée. La correction sépare les traces valides du buffer rejeté, sans modifier les octets ni les compteurs cible ; seule l'analyse hors ligne a été relancée. Le dump officiel de0mots est explicitement vide. Retour7 de qualification : refus SPI réel, pas succès masqué.

## Matériel et état final

Seconde NUCLEO uniquement : SN `0032001F3235510C37333439`, UID `001B00463842501820333157`, vérifiés avant flash. Programmation standard SWD08000000/verify, relecture exacte, reset GDB après dernier CLI, ready/core16/faults0 avant trigger unique. CFG1 MBR=1 confirme le diviseur4 ;4MHz sont calculés, sans mesure oscilloscope. Sessions GDB/serveurs exit0 et detach.

45traces,972octets journalisés,90snapshots ; aucun overflow ou refus du journal. WHO7B et registres07..0A=00/01/0A/86, CTRL1=AC, CTRL3=44, CTRL10=20 relus avant cleanup. Cleanup0 :CTRL1=0C(XLoff),FIFO_CTRL4=80(bypass),CTRL10=00(timestamps off), CFSR/HFSR0. La seconde carte reste sur l'image4 inactive après cleanup. Aucun backup/restauration2MiB, mass erase, Option Bytes ou RDP.

## FRAM : audit des sources uniquement

Le primaire utilise SPI1 :PA5 SCK/PA6 MISO/PA7 MOSI AF5, CS PD14, mode0, source SYSCLK/div64. SystemClock_Config configure160MHz, soit2,5MHz calculés. Freeze D1 qualifie READ/WRITE du MB85RS2MTA à2,5MHz ; D3B conserve les timings historiques et M014-H le provisioning/recovery V2/génération12. Le capteur utilise SPI3, PC10..12 AF6/CS PC9. Les dépendances de production prennent deux handles distincts : aucun partage SPI1/SPI3 imposé par les sources.

La [datasheet constructeur RAMXEED](https://cn.ramxeed.com/home/wp-content/uploads/2024/10/MB85RS2MTA-DS6v0-E.pdf),p14, donne33MHz pour VDD1,7..2,7V et40MHz pour2,7..3,6V sous conditions AC. Cela ne qualifie pas la FRAM TR2 à ces débits. L'ancien lien Fujitsu redirige ; la source RAMXEED a été consultée. Aucun nouveau relevé électrique ou accès FRAM.

Changer seulement le diviseur SPI3 n'impose pas de changer SPI1/FRAM. Le primaire conserve des horloges et paramètres GPIO différents du banc isolé : toute transposition devra être qualifiée séparément. Si une future intégration modifie le bus FRAM ou une horloge partagée, sa fiabilité devra être préservée ou requalifiée. Aucune modification ni essai FRAM dans S.

## Préservation, publication et suite

HEAD initial `7e28d919974f7c24356b88c3ab72f1f5803fb513`, worktreeS isolé. Dépôt principal82169af, anciens worktrees et stashes préservés. Contrat/rapport R archivés byte-exacts depuis4c0785b. Permissions commit/push/flash/debug exercées dans le périmètre du second banc uniquement. Aucun accès matériel au primaire/M014-H/FRAM/SD/partition/Modbus/HP/supervision. Journal et roadmap actualisés :refus4,8NONTESTÉ, acquisQ conservés, M015 IN PROGRESS.

Prochain contrat ciblé : traiter le service RX SPI3 et son budget2µs/octet à4MHz, avec les63octets/snapshots/erreurs existants et les acquis2MHz. Un service plus rapide ou DMA exige un contrat distinct et des gardes équivalentes ; aucune hausse8MHz ni assouplissement de timeout/checks ici.

[Preuves et matrice](preuves/TR2-20261013-M015-S/README.md), originaux `/home/lolo/dev/msm/reports/TR2-mission015-S-20261012T231900Z`. Rapport final et preuves publiés au commit `91845e5f9641af02bad925a3ca7dec0927cc2dbf` (« M015-S: preserve 4 MHz SPI overrun evidence and stop 8 MHz trial »). Les 87 fichiers ajoutés/modifiés depuis le HEAD initial ont été intégralement relus depuis origin/main et sont byte-identiques ; preuve publication-readback.json. Worktree propre après ce commit, dépôt principal et stashes préservés. Statut BLOCKED technique maintenu. Ce complément consigne la publication ; son SHA final sera fourni après sa propre relecture distante.
