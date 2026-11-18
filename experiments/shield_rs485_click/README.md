# Banc RS485 Click isolé — NON-PRODUCTION

Mission `TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL`. Ce candidat ne qualifie aucun
shield : le défaut SPI3 et le NO-GO global E-01 restent présents. Seul le Click
peut être enfiché. Aucune initialisation SPI/FRAM/SD/IIS3DWB, aucun accès média,
aucune identité persistante ou commande métier.

## Préconditions matérielles avant flash/essai

Utilisateur : inspection à froid, autres modules retirés, autre banc hors tension,
mesures TP1/TP2 (DC proche0V avant ohmmètre), absence de court-circuit, masseJ2-11,
présence J5 et continuité **CN12-69→J5local5→J9-3/CS/DE**. Une liaison provisoire
éventuelle exige une décision et un geste physiques distincts ; elle ne prouve pasJ5.
Click : alimentation sélectionnée3V3, pontJ2 DE-/RE, J7 A/Y etJ8 B/Z, TERM/BIAS et
GND2 isolé selon bus réel. Identifier les marquages Data+/Data− du convertisseur.
Puis alimentation contrôlée/mesures3V3 et domaineVDDIO2/PG4/PG7/PG8 avant activité.
Ancien succès du Click sur fils ne dispense pas de ces nouveaux chemins du shield.

Firmware actuel et UID à identifier fraîchement après ce gate. Réutiliser une
sauvegarde vérifiée seulement si la Flash correspond encore intégralement ; sinon
sauvegarder et vérifier avant remplacement. Flash Windows standard qualifié,
adresse0x08000000, SN explicite, readback ; pas HOTPLUG/OptionBytes/mass erase.
Ne jamais flasher avant les mesures requises. L’image001 refuse l’UID002 et inversement.

## Construction et preuve logicielle

Compiler avec le toolchain existant `experiments/iis3dwb_st_reference/cmake/arm-none-eabi-gcc.cmake`,
SDK `/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`, `-DCLICK_BOARD=001` ou002.
Construire explicitement la cible `click_bench`. Les objets nécessaires du cœur
Modbus existant sont liés sans modifier ses sources. Les anciens tests ne sont
pas relancés. `-DCLICK_HOST_TESTS=ON` construit uniquement le nouveau test de
politique d’armement/challenge : absence d’écho, CRC, nonce/UID et refus des entrées
invalides. Cross-build et test ne prouvent pas le fonctionnement sur cible.

Transport partagé inchangé : LPUART1 PG7/8 AF8,115200/8E1 ; DE PG4 bas avant
libération VDDIO2 ; timers T1.5=750µs/T3.5=1750µs, watchdogTX50ms, libération auTC,
réception réarmée et garde aprèsTX. Le flag historique `RS485_TX_QUALIFIED=1`
est activé **uniquement pour cette image de qualification expérimentale**, pas
comme assertion de PASS. Les crédits et l’armement RAM restent obligatoires.

## Séquence bornée, après préflight

Boot : `click_probe.stage=1`, mode0, zéro créditTX ; réception passive seulement.
Les autres GPIO/SPI ne sont pas initialisés. Horloges CPU160MHz etLPUART HSI16MHz,
pas une calibrationSI. Les LED ne constituent pas l’interface de ce banc.

- `click_command=1` : RX seul, aucun armementTX. Envoyer une trameH18? avec le
  script Windows en modeRX. Comparer les14 octets réellement reçus enRAM aux
  octets hôte, erreurs/overflow et absence deTX.
- Puis, bus silencieux/DE bas : `click_arm=0xC118A55A`, `click_command=2` : au plus
 16 réponses RAW, chacune déclenchée par un challengeCRC valide. RéponseT18!
  distincte, même nonce/séquence et UID physique ; aucune émission spontanée.
  ScriptRAW :8 échanges ; vérifier UID RAM/fil et compteurTX, pas seulementCRC.
- Après repos : même armement, commande3 : vrai runtimeRTU et diagnostic existants,
  adresse1 ; seule lectureFC03 est admise. B1/1000/1 renvoie UNKNOWN0 ; les autres
  sources ne sont pas inventées. Écritures refusées avant dispatch. ScriptMODBUS :
 8 requêtes B1 avec réponse complète comparée/CRC ; tests négatifs possibles
  séparément après ces acquis, sans nouvelle écriture ou campagne exhaustive.

Les transitions ne sont permises que DE bas. Ne pas halter/reconfigurer parGDB
pendantTX ou la fenêtre de réponse ; sauvegarder après repos. Crédits consommés
avant tentative ; aucun retry automatique après erreur. RAM : trace2048événements,
RX1024octets,128frontsDE, saturation signalée et crédits annulés. Aucun effacement
des preuves pour obtenir un PASS. Un reset ne peut être justifié qu’après sauvegarde
de l’échec et diagnostic ; il réarme une session distincte, jamais un succès fictif.

Le wrapper `HAL_GPIO_WritePin` enregistre les écrituresDE avec DWT/ODR/IDR ; ce sont
des observations auMCU avec surcoût de sonde, **pas un oscilloscope au Click**.
Continuité froide + tension logique + réponse physique distincte sont nécessaires
pour interpréter son rôle. Aucune certification d’isolement galvanique/haute tension.

## Hôte T1000

La tentative `-File host_probe.ps1` du18/11 a été refusée par la politique
d'exécution PowerShell avant ouvertureCOM. Ne pas la relancer par `Bypass`,
`Unblock-File`, chargement intégral en ligne de commande ou changement de politique.
Les anciens maîtres métier ne conviennent pas à ce banc (lectures B0/commandes).

Le client natif `tools/ClickHost.cs` est construit par le compilateur .NET Framework
Windows existant (`Framework64/v4.0.30319/csc.exe`, références
`System.Web.Extensions.dll` et `System.Management.dll`). Il reste soumis aux
contrôles Windows ordinaires. Son mode `selftest` n'ouvre aucunCOM ; vérifier son
acceptation effective, l'état des protections et l'absence de refus corrélé avant
tout échange. Aucun moyen de signature n'est inventé ni aucune politique modifiée.
Ce client expérimental attend explicitement l'UID001 ; ne pas l'utiliser pour002.
Syntaxe : `ClickHost.exe RX|RAW|MODBUS COM4 NONCE N PHYSICAL-GATE-CONFIRMED`,
avec N=1 enRX et1..8 autrement. LeCOM doit être redécouvert ; le client exige
l'identifiantFTDI BG04KSSWA, pas seulement le nomCOM4. Conserver stdout JSONL
dans un fichier neuf et relever le vrai code de sortie. Armement cible préalable,
comparaisonRAM indépendante et préflight restent indispensables.

`tools/host_probe.ps1` n’ouvre aucun port sans `-PhysicalGateConfirmed`. Redécouvrir
COM et propriétaire avant usage, un seul maître ; adaptateurFTDI BG04KSSW vérifié,
COM ST-LINK exclu. Ce garde n’est pas une autorisation utilisateur inventée.
Le script exige un fichier de preuve neuf, enregistre octets/QPC/UTC, ferme le port
et s’arrête au premier échec sans retry. N’utiliser qu’après armement cible et
préflight démontré ; aucune désactivation de protection Windows si exécution refusée.

Avant tout déplacement : DEbas, émission finie, cibles/handles au repos, GDB détaché,
puis manipulation hors tension par l’utilisateur. Aucun module de stockage réinséré.
