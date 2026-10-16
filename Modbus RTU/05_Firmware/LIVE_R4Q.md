# R4-Q — composition incrémentale LIVE B3 / RTU

Mission TR2-20261016-R4Q-LIVE-B3-RTU-COMPOSITION. R0 et V1 restent inchangés.
Cette tranche prépare la composition ; elle ne sélectionne pas un firmware
LIVE opérationnel, ne qualifie pas R6 et n'annonce pas la capacité LIVE sur la
carte. Le cumulatif canonique reste V1, le probe est explicitement NON-PRODUCTION.

## Écarts et composition retenue

| Interface examinée | Acquis réutilisé | Raccordement / limite R4-Q |
|---|---|---|
| FIFO | propriétaire R3-A, transport ST et parser partagés | binding R3-B existant, même source réelle sur SPI simulé ; pas de deuxième parser |
| Temps événement/UTC | callback par XYZ obligatoire en R3-B | grille temporelle explicitement simulée dans le test ; fournisseur STM32 observé/calibré encore manquant. HAL/DWT sert au scheduler/âge, jamais substitué au temps capteur |
| Calcul/B3 | R3-B et projecteur V1 | snapshot RAM double buffer ; lecture sur copie privée sans callbacks, âge/fraîcheur évalués à chaque lecture ; validité et UTC absente visibles |
| Run/config/génération | autorités R2/R1, fenêtres R3-B | contexte de la valeur publié en RAM, ancienne valeur retenue garde son contexte ; aucune adresse R0 inventée pour run_id |
| B0 | identité protocole2 de R1 | identité préparée inchangée/capacités0x000C ; pas de bit LIVE opérationnel ni SD ; V1 refuse ce profil dans le nouveau serveur |
| B4 | staging/codec/projecteurs existants | écritures staging RAM ; APPLY capture immuable avant admission R2, CRC/domaines revalidés par R2 ; active seulement après commit |
| B5 | vrai moteur R2 et ses29registres | capture RAM FC10, admission/phase durable seulement owner_step ; retry/idempotence et reçus R2 conservés |
| Scheduler | scopes/quanta R4 | service FIFO, publication RAM, réception/réponse ; jamais owner_step dans le callback coopératif. Garde depth/servicing/owner_busy |
| UART/RTU | receiver, CRC, adressage, serial_transport STM32 existants | seule extension du runtime : handler PDU optionnel, NULL conserve le comportement V1. IRQ demeure producteur d'événements, pas d'admission |
| B1/B2/B6/B7 et effets hérités | composants historiques et besoins R0 connus | non composés ici, plages indisponibles/exception02 ; pas de valeurs d'autorité fictives ni redirection vers le journal V1 |

## Propriété et bornes

`LiveModbusServer` est attaché avant la première mesure ; toutes ses méthodes
appartiennent à un même contexte principal, sans thread/ISR concurrent. Les IRQ
produisent seulement les événements du transport qualifié. La publication ne
peut se faire pendant le traitement PDU. Un PDU sélectionne une image publiée et
la copie complète avant de rendre la main ; le transport doit copier l'ADU avant
retour, comme le port STM32 existant. Pas de pointeur emprunté à une fenêtre en
cours de modification. Ce contrat n'est pas une primitive multithread lock-free.

La projection B3 de lecture utilise une copie RAM de métrologie dont les callbacks
sont supprimés. Elle expose l'âge au temps de lecture, sans FRAM/calcul de fenêtre
ni effet matériel. Recul de l'horloge : V/F retirés et faute mémorisée ; arrêt réel
au prochain owner_step (ou garde temps du scope), jamais depuis PDU/IRQ.
Le propriétaire doit publier après chaque poll/invalidation et à chaque quantum ;
une image figée ne peut annoncer indéfiniment FRESH grâce à la projection d'âge.

FC10 acquitte la capture en RAM, **pas** une admission durable ou un succès métier.
Les lectures mailbox reflètent le staging ; statut/last/epoch viennent de R2 publié.
Trois captures normales et une réserve STOP. Plein : exception04 avant mutation,
aucun retry automatique. CLEAR ne détruit pas une requête déjà capturée. CANCEL et
flags invalides sont remis au vrai parseur R2. Le préparé APPLY est copié avec la
requête, pas relu ultérieurement depuis un staging modifiable.

STOP en réserve passe avant les captures ordinaires lorsque LIVE fonctionne et
R2 est libre, sans réordonner une collision de clé. Une transaction durable déjà
admise n'est jamais préemptée : nouvelles requêtes reçues contre elle passent par
les refus R0 de R2, ne sont pas conservées après refus pour exécution ultérieure.
L'alternance capture/phase garantit le progrès de R2 sous trafic ; une faute
réelle exige le chemin d'arrêt indépendant. Pas de garantie de temps métier STOP
par la seule taille de file : chaque phase peut prendre des secondes tout en
servant FIFO/RTU. Le WCET électrique reste à qualifier.

## Validations et essai cible prévu

Test140 : vrai FIFO/ST simulé -> R3-B -> RAM -> RTU pendant vrais stores R1/R2,
quanta FRAM32 ; modèle SPI2,5/4MHz, allowance CPU déclarée12+5us/XYZ, aucune mesure
cible déduite. 1554683XYZ/7019RTU dont4624B3/2312B5, FIFOmax26/RXmax6,
work484us/service498us/gap613us, réponse enqueue max24874us depuis premier octet
(modèle de wire inclus). Fenêtres4 tailles,61mg constant indépendant, UTC présente
ou absente, lectures complètes cohérentes, âge, overflow, déconnexion, file pleine,
STOP réservé, APPLY immuable, replay/reboot sans effet et faute FRAM vérifiés.
140tests complets PASS61,23s et ARM canonique ; sanitizers ciblés. Le nouveau handler
change la disposition du runtime et le BIN V1 : non-régression de comportement
par tests complets, **pas** une identité binaire annoncée.

Probe `TR2_STM32_R4Q_BENCH` : entrée distincte, seul trigger9 ; réutilise les helpers
qualifiés R4-P et uniquement qualify(5) sur snapshot exacta13b…a2ef9. Ownership0,
capteur arrêté, pas de START ni de owner_step, uniquement FC03 B0/B3/B4/B5 sur un
SerialTransport RAM ; aucun périphérique UART/TX activé. Charge readonly FRAM10s,
mesure DWT travail/service/publication/PDU, B3 inactive/V0/F0, capacités préparées.
Cela ne qualifie ni l'électrique ni le FIFO avec calcul physique ; fournisseur de
temps événement absent, aucune nouvelle transaction FRAM pour contourner cela.
Après essai : snapshot FRAM entier exact, Flash initiale restaurée et relue2Mio ;
aucun rollback epoch/run ni OptionBytes. Résultats effectifs et SHA dans le rapport.
