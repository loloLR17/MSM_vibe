# M013-G — plan temporel et campagne de robustesse limitée

11 octobre 2026. **PRÉPARÉ : READY outillage logiciel HP hors bus ; WAITING instrumentation physique identifiée. Aucun GO d'émission en G.** L'image diagnostic E existante suffit : observerON/TXON/unité1, FC03/1000/1 UNKNOWN0 uniquement. Aller-retour unique E corroboré par F ; son instruction est consommée. Ce plan ne modifie ni V1 ni les permissions matérielles. Une future mission et revue ChatGPT du GO précèdent toute capture/branchement/campagne réelle.

## Moyens établis et limites

| Moyen | État vérifié / déclaré | Utilité et limite |
|---|---|---|
| HP / FT232R COM7 | Utilisateur E/F : 1152008E1, réponse exacte observée | Journal PC, essais fonctionnels bornés ; pas chronométrie submilliseconde fiable par USB/OS |
| STM32 / ST-LINK Windows | Identifiés E/F, observer RAM et ELF exact conservés | Deltas IRQ/RX/RTU/TX/TC et état final ; absence de timestamps haute résolution et de capture DE continue |
| Oscilloscope / analyseur logique | Aucun modèle, canal, bande passante ou disponibilité documentés dans les sources consultées | WAITING identification ; ne pas conclure à leur absence physique |
| Sonde différentielle / isolement | Aucun matériel ou domaine de tension admissible établi | WAITING ; capture A/B reportée sans branchement improvisé |
| Python / pySerial sur HP | NON VÉRIFIÉ ; outil testé Python3.13 Linux, mocks sans bibliothèque série | Vérifier Python>=3.9 et pySerial3.5 sur HP hors port, avant tout GO de campagne |

Aucun inventaire physique supplémentaire n'a été demandé pendant G. Aucun oscilloscope/sonde fictif attribué au banc. RE/RST/VIO/J1/J2/J3/SB28 et câblage acquis ne sont pas à recontrôler sans preuve contradictoire.

## Références et lecture du code

La spécification Modbus Organization Serial Line V1.02 §2.5.1.1, pages12–14, recommande au-dessus de19200bit/s t1.5=750µs et t3.5=1750µs. En 8E1, 11bits/caractère : à115200bit/s, bit8,6806µs, caractère95,4861µs, requête8octets763,889µs, réponse7octets668,403µs hors silences. Source primaire auteur Modbus, [copie publiée ANL](https://millenia.cars.aps.anl.gov/software/epics/Modbus_over_serial_line_V1_02.pdf) ; URL historique modbus.org/docs désormais404 lors de G. Valeurs calculées nominales, aucune mesure du banc.

Transport actuel : TIM6 à1MHz, T1.5=750 puis complément1000µs, T3.5 consommé avant autorisationTX ; file vide/RXNE/BUSY absents, RX aborté avant PG4DEhaut. HAL démarre TX IT, watchdog50ms. Callback HAL TXCplt issu de TC remet DEbas, réarmeRX et lance garde1750µs ; garde expirée retourneIDLE sans émission synthétique. IRQ callbacks/compteurs prouvent exécution, pas durée physique. La dérive d'horloge réelle, latence IRQ, propagation isolateur et seuils de mesure restent inconnus. Pas de pulseTC ajouté au firmware : TC sera corroboré indirectement par callback et fin UART physique, sans prétendre voir le registre TC sur une broche.

## Protocole physique futur, conditionné au matériel identifié

L'utilisateur raccordera les instruments seulement sous mission dédiée : références/modèles/sondes, tension/common-mode admissibles, masse/terre/isolement et points accessibles doivent être identifiés d'abord. Plan proposé de points logiques : PG4/DE, PG7/TXD, PG8/RXD, référence GND1 logique, uniquement sur accès vérifié. Ne pas déplacer jumpers ni ponts existants, pas d'ajout de terminaison ou charge non qualifiée.

Une masse d'oscilloscope reliée à la terre peut ponter l'isolation. Ne pas supposer masse HP = GND1 = masse bus ; ne pas accrocher une pince de masse sur A ou B, ni réunir GND1 et masse isolée à travers les instruments. Capture différentielle A–B seulement avec sonde différentielle ou entrées réellement isolées, plage common-mode et sécurité attestées par documentation instrument. Deux canaux avec pinces sur A/B ne constituent pas une sonde différentielle sûre. Si impossibilité, conserver captures logiques et marquer A/B NON MESURÉ.

Ordre prévu : préparer instrument hors banc ; acquisition passive logique DE/TXD/RXD, puis A–B si voie sûre disponible. Pour corrélation complète, quatre voies simultanées (3logiques+1différentielle) ou acquisitions synchronisées avec horloge/trigger et incertitude inter-instrument établis. Avec moins de voies, captures partielles sur essais distincts explicitement autorisés : elles ne prouvent pas simultanéité d'un même échange.

Proposition instrument : échantillonnage>=10MS/s pour UART/DE, enregistrement>=30ms par échange et résolution mémoire suffisante, trigger DEhaut/prétrigger>=5ms ; premier essai nominal futur unique à définir au contrat, pas E rejoué implicitement. Conserver fichier natif et export brut, fréquences, seuils, calibration, deskew et déclenchement. Horodatages du PC ne remplacent pas cette acquisition. À10MS/s, quantum0,1µs ; calculer borne U incluant deux quantums, skew, précisionbase de temps et choixseuil. Si U>0,5µs, verdict des petites marges DE reporté, même si les octets se décodent. Budget proposé de préparation, pas propriété d'un appareil présent.

## Critères mesurables proposés au futur contrat

Ces bornes de banc doivent être confirmées avec l'instrument réel ; elles ne sont pas de nouvelles spécifications V1. Ne pas faire passer une mesure située dans sa bande d'incertitude : succès seulement si la borne inférieure/ supérieure appropriée satisfait le critère ; sinon INCONCLUSIF ou échec certain si hors intervalle.

| Mesure / critère | Seuil proposé et incertitude | Preuve exigée |
|---|---|---|
| UART | 1152008E1 ; bit8,6806µs ±2% comme tolérance de banc, parité et octets exacts | Décodage logique et A/B si disponible, 8octets requête/7réponse/CRC0, aucune troncature dernierstopbit |
| Intertrame requête→réponse | Fin dernierstopbit RX jusqu'au premierstartbit TX >=1750µs, avec borne inférieure Δ−U | Chronogramme, pas timestamp du script ; code timer attendu750+1000µs mais ISR peut retarder |
| Continuité trame | Pas de silence intercaractère >=750µs (borne supérieure <750µs) | Décodage de chaque caractère et intervalles ; pas limite1.5caractère appliquée abusivement à haut débit |
| DE avant TX | Δ(DEhaut→premierstartTX)−U >=1µs, marge de banc proposée | Capture logique simultanée ; vérifier propagation/enable ADI avec conditions réelles avant qualification électrique définitive |
| DE maintenu | DEhaut pendant tous les bits jusqu'à fin dernierstopTX ; aucune troncature A/B | Capture continue ; événementTC RAM seul insuffisant |
| Libération DE | Δ(fin dernierstopTX→DEbas) dans[0,100µs], intervalle d'incertitude inclus | Critère de banc proposé, pas limite normativeHAL ; mesurer et rapporter toute marge insuffisante |
| RetourRX | RXD restitue intégralement la prochaine requête autorisée après DEbas ; RxStateBUSY_RX et callbacks cohérents | Capture RXD + deltas ; campagne100ms ne prouve pas reprise au délai minimal1750µs |
| Latence / watchdog | Réponse nominale PC exacte dans fenêtre1s ; TX fini sans failsafe ; mesurer largeurDE et rapporter valeur | 1s seuil fonctionnel hôte, pas exigence normativeModbus ; watchdog50ms non qualifié sans essai de panne dédié |

Une épreuve au voisinage du délai minimal de retourRX nécessitera générateur/maître à timing matériel identifié et mission séparée ; Windows/USB ne garantit pas un stimulus à1750µs. Pas d'injection de panne/IRQbloquées/reset ou test endurance dans G. Ne pas déduire amplitude/common-mode/conformité électrique générale d'un simple décodage A/B.

## Outillage HP dédié : rtu_campaign.py

Dossier `05_Firmware/platform/host/rs485_rx_probe/`. Script historique Send-Tr2RxProbe.ps1 inchangé. Nouveau Python avec import pySerial seulement en mode physique ; interface [pySerial3.5 officielle](https://pyserial.readthedocs.io/en/latest/pyserial_api.html) consultée. Port COM7 fixé,1152008E1, sans handshake/DTR/RTS, lecture non bloquante, écriture bornée1s. Certaines interfaces peuvent faire transiter RTS/DTR à l'ouverture malgré configuration : branche matérielle non testée, vérifier compatibilité à la future revue, aucune modification du banc supposée.

Sans options : OFFLINE imprime le plan et n'importe pas pySerial. `--mock` utilise uniquement objets mémoire et horloge simulée. `--transmit` exige Windows, marqueur non vide `--authorized-mission` et nouveau journal `--log` ; ce marqueur trace un contrat, ne crée pas l'autorisation. Aucun port configuré avant sélection physique explicite. Ne pas lancer ce mode dans G.

Commandes hors bus, depuis dossier du script :

```text
python rtu_campaign.py
python -m unittest -v test_rtu_campaign.py
python rtu_campaign.py --mock --log nouveau-mock.jsonl
```

Préflight HP futur hors port : `python --version` puis `python -c "import serial; print(serial.__version__)"`. Dépendance prévue pySerial3.5 ; sa disponibilité et le PythonHP ne sont pas établis en G. Installation éventuelle dans environnement dédié et contrôle de version relèvent de la préparation future ; pas de modification de politiquePowerShell/configuration globale. Outil READY hors bus, exécutionHP conditionnée à ces vérifications et contrat.

Commande **réservée à une future mission d'essai autorisée après revue GO ChatGPT**, pas à exécuter maintenant : `python rtu_campaign.py --transmit --authorized-mission <identifiant-futur> --log nouveau-physique.jsonl`.

Plan fixe, aucune boucle illimitée : nominal4, CRCinvalide1, nominal2, unitéétrangère2 unefois, nominal4. **12 écritures maximum, 10 nominales, 2 négatives**, pas broadcast/écritureV1/fonctions production. CRCinvalide :010303E80001047B ; unité2/CRCcorrect :020303E800010449. Précondition : un seul maître HP et une seule cible diagnostic unité1, aucun autre répondant unité2 ; sinon NO-GO pour ce plan. Intervalle observation1s depuis intentécriture, puis attente100ms après chaque observation ; garde supplémentaire100ms avant première requête. Budget nominal~13,3s, arrêt avant nouvelle requête si20s atteintes. Lecture limitée256octets par essai, timeoutwrite1s ; aucun retry ou purge RX silencieuse.

Journal JSONL créé exclusivement (pas d'écrasement) et flush avant chaque écriture ; UTC+tempsmonotone relatifs, intentécriture/nbécrits, tous chunks reçus, trame complètehex, résiduCRC, verdict/timeout, premierRX côtéhôte et fenêtre observée, marqueurmission/simulated. Latence premierRX mesure livraisonUSB/OS, pas fronts physiques ; aucune suppression d'écho ni extraction opportuniste de bonne trame dans bruit. Le journal mock porte simulatedtrue/MOCK_ONLY, jamais preuve physique.

Nominal réussi uniquement si octets exacts0103020000B844, CRC0, aucune donnée supplémentaire. Négatifs : silence dans fenêtre1s, puis nominal suivant réussi. Tout écho brut/écho+réponse, bruit/trame partielle/CRCincorrect, exception, réponseunitéétrangère, nominaltimeout, RXtardif/préexistant, overflow>256, écriturepartielle, exceptionI/O ou écheclog → arrêt et fermetureport, aucun nouvel envoi automatique. RXpréexistant conservé jusqu'à256octets dans log (pas de purge), campagne arrêtée. Si écho connu de l'adaptateur, campagne INCONCLUSIVE et arrêt, pas suppression sans qualification distincte.

## Preuves attendues d'une future campagne

Avant et après : image E exact/ELF et chaîneGDBseule, baseline RAM conservée sans reset, NVIC49/66 sains, ready/polling/RXarmé/PG4bas. Pas de GDB/halt pendant campagne. Préserver JSONL et sorties/snapshots sans reflash/reset avant analyse. Ne pas rouvrir CubeProgrammer ; si indispensable, conserverRAM puis procédureD dans mission autorisée.

Pour campagne complète12 : deltaRXoctets96, trames12, CRCvalid11/CRCbad1, foreignunit1, accepted/available10, TXattempt/started/TCcallback10, aucune erreur/drop/failsafe, queuevide/TXidle/RXréarmé/NVICsain. IRQ UART/TIM6 progressent mais aucun nombre précis imposé sans caractérisation ; poll/tick continuent sans reboot. Compteursmodulo2^32, calculdeltas modulo, considérer preuveambigüe en cas de reboot/wrapmultiple inconnu. Si arrêt anticipé : comparer seulement préfixe réellement écrit et conserver toutes limites ; pas d'extrapolation aux12. Dernièretrame RAM ne remplace pas journal des précédentes.

Cette tranche mesurerait répétabilité limitée10nominales et récupération après deux rejets, pas endurance, production, limites temporelles minimales ou tous défauts. Robustesse panneTX/IRQbloquées/watchdog/reset devra être une autre mission, avec garde-fous propres. Étape suivante minimale ChatGPT : revue outillage/rapport, état environnementHP horsbus et instrument réellement disponible ; décider mission limitée HP et/ou capturesphysiques reportées. Aucune manipulation demandée dans G.
