# M015-D — Diagnostic discriminant de lecture FIFO active

## Acquis réutilisés

B : 171 statuts, 11 reculs avant toute lecture de données, premier127→0 ; FIFO_CTRL2 vérifié81 puis relu80 ; overrun brut non qualifié. C :512 lectures cohérentes au repos (WHO7B, CTRL3=04, FIFO vide), mode3/625kHz, même framing. Ces campagnes et leurs tests inchangés ne sont pas rejoués. Les motifs bit7/décalage sont des hypothèses ; aucune donnée n’est corrigée.

[AN5444 §§4.4/6](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) et [DS12569 §§3.2/4.3/9](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) : R/W bit7, adresse7bits, auto-incrément IF_INC, commande puis donnée sans dummy supplémentaire, CS bas pendant tout le transfert. BDU impose3A→3B ; niveau10bits. FIFO_CTRL2=81 limite le modeFIFO1 au seuil256. Bypass remet la FIFO à zéro. Les flags ne sont pas qualifiés à partir d’une lecture incohérente. Le code respecte masque03FF et ordre ; pas de nouveau changement mode/fréquence.

## Test unique, sans vidange

| Hypothèse | Discriminant | Limite |
|---|---|---|
| Lecture courte/burst incohérente | Comparer statuts3A/3B groupés et deux singles successifs, CTRL2 single et bloc07..0A | Pendant remplissage, tenir compte du temps entre lectures ; égalité exacte non requise |
| Problème propre au compteur dynamique/BDU | Couper CTRL1 après remplissage, garder FIFO mode1, comparer32 groupes sans DATA_OUT | Stabilisation2ms ; pas preuve de conversion ou consécutivité XYZ |
| Corruption des octets hauts/configuration | Journaliser WHO7B, CTRL3=44, CTRL2=81 en alternance avec statuts | Les readbacks seuls ne mesurent pas le signal électrique |
| Défaut de CS/flags HAL | TX/RX bruts, marqueursODR CS, DWT, SRavant/après, CFG et compteursHAL | Pas capture wire ou flags internes continus ; analyseur logique si nécessaire |

Un seul profil dédié OFF/exclusif, trigger4D313544. Sept contrôles volatils sauvegardés, état préexistant actif refusé ; configurationmodeFIFO1/STOP_ON_WTM256/BDR26.667/±4g/BDU. Après100ms de stabilisation capteur en bypass, activationFIFO puis groupes7lectures pendant15ms maximum/borne700records, arrêt surWM lu. Coupure entrée CTRL1 sans vider,32groupes figés puis restauration des7contrôles. Borne totale1024×80octets ; réserves pour restauration. Pas acquisition continue/overrun volontaire/lectureFIFO_DATA_OUT/XYZ/FRAM/SD/UART.

Le statut physique et la cause ne sont pas déduits de HAL_OK. Une fiabilité active exige statuts cohérents, valeurs constantes attendues, progression compatible avec temps, et stabilisation sans reculs inexpliqués. Une anomalie de constante exclut la seule dynamique du compteur ; byte correct sur fil mais faux enRAM permettrait isoler réception/HAL. Sans discrimination concluante, BLOCKED.

## Validation ciblée

Test hôte neuf : nominal, anomalie bit7 conservée dans la trace, mismatch configuration refusé avec restauration, capteur préexistant actif refusé sans écriture ; aucun accèsDATA_OUT, une seule activationFIFO, CS et RXcommande conservés. Première simulation ratée : le niveau128 tombait sur lecture simple et pas sur burst prévu ; injection au premier burst >=128 et vérification par index exact corrigées, sans affaiblir les critères. Test final1/1 et cross-build défaut/profil réussis. Cœur/driver et testsA/B/C/.NET inchangés nonrejoués ; warningsNewlib conservés.

## Campagne D acquise — aucun rejeu

468 transactions, HALresult/restore_result/errors=0 ; phases1/2/3/4/5 :9/16/196/226/21records. Phaseactive :28groupes en10ms jusqu’àWM256. Aucun DATA_OUT/XYZ ; sept contrôles restaurés0/0/0/0/0/4/0. HAL_OK est un résultat de transport, pas une qualification des octets.

| Readback | FIFO alimentée | Après entrée coupée et2ms |
|---|---|---|
|CTRL2 single|80×23,81×2,02×2,00×1|81×32|
|CTRL2 dans burst07..0A|81×28|81×32|
|WHO single|7B×28|7B×32|
|CTRL3 single|44×28|44×32|
|Statuts groupés|1→256,28lectures, aucun recul|A100×32 ; singles concordants32/32|

26/28 comparaisonsCTRL2 divergent durant remplissage,0/32 après coupure. Aucune écritureCTRL2 entre cescomparaisons ; burst07..0A donne toujours00/81/0A/01. Fait discriminant : readback single du registre08 dépend du contexte alimenté/figé ; le mouvement ne peut modifier la valeur attendue d’un registre de configuration. Pas preuve wire de la vraie valeur ni cause électrique précise.

À proximité du passagebit7 : record116 `BA0000`→`007F00` (127),117 `BA00`→`0081` et118 `BB00`→`FF00` (129),119 `8800`→`0080`,121 `8700000000`→`FF00810A01`,123statut137. Aucun recul127→0 observé dansD. Niveau128 exact non échantillonné : ne pas prétendre une mesure de tous les niveaux.

Cadence médiane des statuts groupés :B54,725µs contreD364,575µs, avec interleaving et writesHALduplex enD. Pas essai àvariableunique expliquant le fastpollB. Les singles STAT2 sont0 sauf20 puisA1 àla fin ; paireA100 contientWMT/FULL, pas bitsOVR_IA/LATCHED. Ces observations ne valident pas le mécanisme d’overrunB. Pas de nouvelle ODR/consécutivité/pertes qualifiées.

**BLOCKED** pour cause précise et fiabilité de toutes les lectures FIFOactives : un chemin de readback est clairement non fiable, mais la causeSPI/MCU/capteur n’est pas isolée. Aucun contournement du single par burst enproduction, aucune correction de donnée, aucun deuxième profil ou essai. La fiabilité apparente des28statuts espacés ne résout pas le défaut des171fastpollsB.

## Geste de mesure préparé, non exécuté

La preuve discriminante suivante exige des tracesCS/SCK/MOSI/MISO décodées et comparées auRX brut des mêmes commandes88 et87 pendant remplissage, puis étatfigé ; observer aussiBA. RéférencesGPIO connues :PC9/10/12/11, GNDlogique capteur. Connexion uniquement par utilisateur avec analyseur compatible3,3V, entréeshauteimpédance et masse logique identifiée ; pas alimentation/outrouteRS485isolée. Instrument non identifié : ne pas demander de brancher à l’aveugle. Au moins10MHz pour décodage625kHz, résolution supérieure pour qualifier setup5ns/hold20ns. Voir protocoleC conservé pourpremier/dernierbit,CScontinu16/24/40clocks etabsence dummy. Aucun branchement/capturewire exécuté. Cible déjà restauréeM014 : une nouvelle exécution ou flash requiert un nouveau contrat court, pas relance impliciteD.
