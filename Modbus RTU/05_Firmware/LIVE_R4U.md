# R4-U — composition LIVE opt-in, qualification NON-PRODUCTION

Le runtime portable `live_runtime.[ch]` compose le propriétaire FIFO unique,
provider daté observé R4-S, R3-B, serveur snapshots et vrai moteur R2/R1.
Chaque service draine FIFO/horloge, publie au plus une image toutes les10ms et
traite au plus8 événements UART. Aucun step R2 ni admission dans ce service.
Le foreground fait un enter/leave coopératif puis au plus un owner_step R2 ;
les callbacks de commits continuent à servir FIFO et UART sans réentrée.
Les ISR UART mettent en queue et terminent TX/DE ; aucune FRAM dans une lecture.

## Matrice vérifiable B0–B7

| Bloc | Composition/autorité R4-U | Limites observables |
|---|---|---|
|B0 0..20|identité persistée R1, projection existante|DeviceId1, protocole2/capabilities12 ; bitLIVE16 absent, sous-ensemble NON-PRODUCTION, aucun produit complet annoncé|
|B1|non supporté, exception02|température/CPU/horloge système non composées, pas de valeurs inventées|
|B2|non supporté, exception02|autorité RTC/UTC non composée|
|B3 3000..3047|snapshot R3-B N4096, timestamp FIFO/provider réellement calibré|invalidité avant fenêtre, V/F seulement autoritatif, HELD/F0 après STOP, UTC absente ; grands N non qualifiés|
|B4 4000..4175|R1 configuration/config3/gen3 et staging existant|écritures seulement selon codec/contrat existant ; pas de campagne mutation configuration ici|
|B5 5000..5028|capture RAM puis moteur R2 différé durable|epoch5, prochaines clés3 START/run7 et4 STOP ; retry sans effet, collision27 ; aucun recyclage/retraite|
|B6 6000..6063|option explicite R0-INV UNAVAILABLE : schema2,health4, sélection invalide|sélecteur6003 RAM uniquement, aucun stockage/SD ni inventaire vide fabriqué|
|B7|non supporté, exception02|journal durable fautes non composé|

DefaultV1/profils antérieurs inchangés. Le serveur n'active B6 que sur demande.
Le mode lecture seule refuse toute FC10 avant capture/staging. Les143 tests
antérieurs sont préservés ; nouveau test runtime asynchrone144, sanitizer ciblé.
Le helper de test existant est exporté sans suppression d'assertions.

## Banc et plan physique conditionnel

TR2-001 UID004A003D/38425018/20333157, ST-LINK003500463235510B37333439.
T1000 COM3=STLINK VCP ; COM4=FTDI VID0403/PID6001/BG04KSSW, confirmé par
l'utilisateur comme adaptateur USB-RS485 correctement raccordé au TR2.
LPUART1 PG7 TX/PG8 RX AF8 ; PG4 DE-/RE commun ; PG5 inutilisé.
115200 8E1, TIM6 1MHz, t1.5=750µs/t3.5=1750µs ; transport IRQ/TX asynchrone
existant réemployé. La confirmation de raccordement ne mesure pas la terminaison.
QualificationV1 antérieure n'est pas une qualification LIVE physique.

Image `tr2-r4u-live-runtime-NON-PRODUCTION`, flagR4U explicite, incompatible
R4T_RECOVERY. Boot WAIT, aucune acquisition/écriture automatique. Préflight frais
Flash2Mio c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26,
FRAM262144 371a9d16c6285d0150608751ccfdd5217ff303d42d8e5446d66a8130342e6634.
La première phase exige exactement cette FRAM et journal38/root51/epoch5/run6
terminal. Aucun helper de retraite appelé/lié.

Phases sous breakpoints seulement à l'arrêt : trigger52345501 RAM5s après
calibration2s, FRAM inchangée ; trigger52345502 UART readonly10s avec lectures
maître5s, FRAM inchangée ; seulement si budgets conformes trigger52345503 autorise
les vrais START5/3 etSTOP5/4 viaCOM4, retry3 identique etcollision3/STOP27.
Le maître ne réessaie jamais une erreur transport/CRC. Une nouvelle clé après
STARTED ambigu est interdite. Capture ready avant START et checkpoint aprèsSTOP,
sans GDB au milieu de l'acquisition. Plafond600s, STOP sûr sur erreur.
Bornes work1000/service2500/gap5000µs inchangées ; contrôles datation100µs.
Au terme, sauvegarder/auditer FRAM entière, restaurer seulement FlashM016 par
pages normales et relire2Mio ; jamais rollback FRAM, SD, mass erase ouOB.

## Entrée R5

R5 doit lire B0 et refuser le mode produit LIVE faute du bit16 ; un outil de
qualification explicite peut utiliser le sous-ensemble décrit. Lire une ADU B3
entière cohérente, contrôler status/flags/âge/UTCabsente et les autorités B0/B4/B5 ; aucun timestamp
UTC synthétique. Après STOP conserver HELD mais jamais FRESH. Les écritures B5
exigent epoch/clé neuves et reçus durables, les réponsesFC10 ne sont que captures.
Les identités run/config conservées dans LiveModbusImage sont internes : aucun
registre B3 réservé n'est réaffecté pour les exposer. La corrélation run7/config3
est démontrée ici par GDB/FRAM et oracle ; ne pas attribuer une preuve causale
complète sur fil à B3 seul. R5 devra définir cette corrélation observable dans
son contrat, sans modifier silencieusement les spécifications gelées.
SQLite/Web/servicePC produit et domaines manquants restent distincts.

Résultats physiques à ajouter après observation, aucun PASS anticipé.

## Correction du harness après image01

RAM image01 :132602XYZ/505ADU,0write, FRAM entière371a inchangée ; work335/service1673/gap1641µs. GDB assert final utilisait work.fault inexistant après dump : observations acquises conservées sans refaire cet essai.
Transition UART refusée avant initialisation (g_serial.uart.Instance=NULL) : tentative de second bind_event_metrology, garde single-use légitime. COM4 ouvert/une requête identité sans réponse, aucune commande B5 et aucune mutation FRAM. Le contrôleur confondait checkpoint d'erreur avec ready ; terminé le seul GDB du test en attente, cible arrêtée auditée.
Image02 : ne touche aucune garde des API ; réutilise liaison unique arrêtée, conserve callbacks, boot réel R2 puis serveur réinitialisé. Le contrôleur vérifie stage/error avant signal ready, et le maître journalise chaque requête avant envoi, y compris timeout. ARM PASS, BSS654560/text145612/data492. Repassage RAM court justifié par cette correction de composition, pas par un besoin de répéter un acquis inchangé.

## Isolation des diagnostics par boot (image03, plan remplaçant les transitions)

Image02 RAM PASS132618XYZ/503ADU, work336/service1613/gap1616µs, FRAM exacte.
UART initialisée mais second START volatile refusé par `run<=s->run_id` : même identité UINT64_MAX-1, aucun sample nouveau/serviceUART/écriture FRAM ; requêteCOM4timeout à nouveau. Ce refus n'est pas une preuve de panne RS485. Ne pas modifier les gardes ni remettre le compteur à zéro en cours de boot.
Image03 : WAIT accepte01/02/03 et exécute une seule phase par boot.01RAMacquise ;02UARTreadonly avec son propriétaire volatile neuf, arrêt au checkpoint ;03sur boot distinct pour le vrai R2/run7. Avant chaque phase, monter exactement371a et calibrer le provider via diagnostic séparé ; pas de deuxième START diagnostic ni de réutilisation du propriétaire volatile pour le run durable. Reset logiciel nécessaire seulement entre ces deux essais isolés, pas de rollbackFRAM ni répétition deRAM. Guardes identités/stores inchangées.

## Essai UART readonly image03 : réponses physiques, défaut sûr

73requêtes COM4,72réponses CRC/longueur exactes : B0=1/B3=36/B6=35,12,47..39,47ms (mesure maître, pas WCETIRQ). Puis arrêt sûr à131050XYZ, FIFOerror9/BENCH_TAG,reason1, parité du tag23 au mot11 du burst161octets. Les12tags suivants se réalignent par décalage d'un bit, timestamp+24ticks reconstruit ; analyse seulement, jamais injection/correction de données. HAL complet/OVRabsent ne détectait pas ce défaut ; tous contrôles maintenus. Work338/service1452/gap1386µs, UART584RX/72TX/0erreur/0drop/0failsafe ; FRAM entière371a inchangée, journal38/root51. Aucun STARTB5 durable.
La cause électrique/ISR/horloge n'est pas établie par ces observations. Essai comparatif image04 : BENCH_DUPLEX=1 uniquementR4U (voie existante command+payload dansHAL_SPI_TransmitReceive, RXpreclose/OVR/parité/config inchangés). Aucun masque IRQ ajouté. RAM5s justifié par nouvelle méthode SPI, puis UARTreadonly10s sur boot distinct. Si cette garde ne passe pas, conserver le défaut sans engager B5. V1 et anciens probes restent BENCH_DUPLEX=0.

## Duplex : garde préalable aux vraies commandes acquise

Image04/code806d967 : RAM132613XYZ/5s, work295/service1483/gap1488µs,0write.
Puis boot distinct readonlyUART10,000207s,265441XYZ/33181TS,79requêtes/réponsesCRCexactes, V/F0 diagnostic honnête ; work294/service1259/gap1263µs, START804/STOP203µs, maxFIFO38/résidumax10µs. FRAM entière371a exacte, journal38/root51,0mutation. Ce succès borné du duplex n'établit ni la causalité exclusive de l'ISR dans le défaut split ni l'endurance.
Étape suivante autorisée sur boot neuf : vrai START5/3/run7, retryidentique3, collision3/STOP27 etSTOP5/4. Modèle RAM de métadonnées attendu journal46/root59, effect27 ; aucune de ces écritures n'est encore exécutée.

## Résultat final physique — 17/10/2026 UTC

Image04/code806d967, BIN147320octets SHA89951ff52ee997ff742ce0684fdbaff1cb22baef080aca872f9dd4e802056f63,
ELF SHA68e666ba776e654d5a4fb0f4178e91250ac0bd235d4444001ddf20b993aa6383,
text146828/data492/BSS657384. START5/3/run7 COMPLETED4/0, retry3 sans nouvel effet,
collision3/STOP refus27, STOP5/4 COMPLETED4/0.3045698XYZ/380713TS,58fenêtres4096 ;
1632réponses physiques CRC/longueur exactes,731V/F,49HELD, avant fenêtre invalide,
UTCabsente. Oracle première4096 : RMS958/955/75/13mg, crêtes1003/998/107/55mg,
durée154ms, égalité exacte auxXYZbruts.1141contrôles horloge, résidu≤100µs.
Work810/service1669/gap1897µs sous1000/2500/5000 ; FIFOmax56/erreurs0,
UART0drop/0erreur/0failsafe. Coût callbackSTART805/STOP203µs.
**Latence commande durable distincte : START63,100s, STOP63,249s jusqu'au terminal
observé ; ACKSTOP→premierHELD55,757s.** Ce délai comprend les barrières R2 et le
polling/USB ; aucune borne d'arrêt temps réel ou de WCETIRQ isolé prouvée.
Priorité de file STOP testée sur modèle, aucune campagne physique de concurrence
de plusieurs maîtres ni préemption des commits. Ne pas assimiler203µs à la latenceB5.
8commits/419408octets/13112writes bornés LIVE,24plages ; FRAM entière exacte au
modèle métadonnées, journal46/root59/epoch5/run7/config3/gen3/effect27phase3,
SHA95dab7909dbbab0a85abf1a3d9f8292bafb48dc98de73f026ce2786251b9af41.
Historique et tail byte-identiques ; aucune retraite, SD ou ancienneFRAMrestaurée.
M016 restauré10:00:31UTC via18pages, Flash entière2Mio c5bc…f5b26 exacte, FRAM
entière finale relue et inchangée ; contrôles capteur WHO7B/ODRoff/FIFObypass/TSoff
après restauration, GDBdétaché, COM4fermé. Le firmware de qualification n'est pas laissé.

Portée limitée à ce banc/charge/N4096. N8192/16384/32768 physiques non acquis :
collecte brute32768 naïve ajoute172032octets, BSS829416>SRAM786432 avant pile.
Prévoir collecte de référence bornée/streamée dans une mission séparée, pas
augmentation tacite SRAM ni modificationV1. Température/CPU/RTC/B7, UTC, power-cycle,
endurance, défauts HAL/CPU/pile, terminaison/CEM et cause du décalage split restent
non qualifiés. Aucun GO produit LIVE/R4global/R5/R6, contrôle indépendantChatGPT attendu.

Clôture de livraisonR4-U après relecture du bilan941e8de depuisGitHub :259fichiers/245preuves/178artefacts exacts,1632ADUparsées. StatutDONE limité au contrat démontré ; contrôle indépendantChatGPT distinct, pas de tag/R4global/R6 ni nouvel essai à la clôture.
