# R4-R — temps événement physique, banc isolé

Mission TR2-20261016-R4R-PHYSICAL-TIME-B3, initial9dc28db. Première tranche :
probe explicite NON-PRODUCTION, capteur/temps seuls pendant2s ; aucune entrée
UART/SD/commande R2, aucune publication B3 avant justification du temps.
Préflight complet Flash2Mio c5bc344d…f5b26 et FRAM262144 a13b3401…a2ef9,
UID/STLINK qualifiés. L'archive R4-Q est byte-identique à650aac8.

Le probe réutilise propriétaire/parsing/gardes SPI qualifiés ; contrôle statuts,
saturation, tags et intervalles TS/8 inchangés. Le numéro diagnostic volatile
UINT64_MAX-1 n'est ni un run R2 réservé ni une autorité produit ; aucune identité
persistante n'est consommée. Le premier mot a une allowance expérimentale20ms,
distincte du gap5ms. Chaque service est mesuré et arrêté au-delà2500us.

Collecte prévue : premier préfixe2048mots bruts, tous les deltas timestamps,
compteurs d'association TAG_CNT et champs réservés, compteur interne lu avec
bracketing DWT toutes10ms. Le probe conserve les états et s'arrête ; aucune
validation physique n'est encore revendiquée dans cette première version.
L'analyse indépendante Python fournit ses propres oracles LE32/rollover/parité
et pente ; aucun nominal26667 ne sert à fabriquer un événement. Bibliothèque
ST et parseur V1 ne sont pas modifiés.

Références constructeur consultées : ST DS12569 Rev8 §9.28 (timestamp/fine),
AN5444 Rev3 §6.2/6.6 (slots, TAG_CNT et corrélation hybride). Une pente sensor→DWT
mesurée et une association observée restent nécessaires ; UTC non synchronisée.
Le raccordement B3, la charge FRAM et les budgets conjoints attendent ce résultat.

## Checkpoint physique capteur/temps — 16 octobre 2026 UTC

Code0e808f3, profil R4R explicite ;141tests hôte62,28s/ARM et ASan+UBSan du
provider PASS. CanoniqueV1 BIN identique85b1be60…c0f5 ; non flashé.
Premier essai8af2451 : relecture GDB trop tôt, après52097XYZ avant fin2s,
HAL SPI interrompu, erreurFIFO1/arrêt235us. Capture conservée, jamais PASS.
Le nouvel essai utilise un point d'arrêt après arrêt du capteur, pas de halt actif.

Diagnostic2,000064s :52894XYZ/6612TS, niveau13, poll401us/gap425us,
premiermot10875us. TAG timestamp AVANT XYZ du même compteur0..3 ;228ancres du
préfixe2048mots vérifiées indépendamment, intervalles24ticks/8XYZ, champs
réservés/cadence0/0xA ; anomalies0, stop/readback conformes.197lectures compteur
bracketées DWT ; fit après100ms :1896543us/151233ticks =12,54054us/tick,
bracketmax33us/résidumax22us. Fine lu1 sans écriture ; la correction nominale
constructeur n'est pas une référence absolue. Ces horloges internes ne prouvent
ni fréquence SI exacte, ni UTC, ni retard analogique du filtre.

Provider opt-in : tampon8XYZ, émission seulement au timestamp suivant, instants
interpolés entre deux ancres observées, TAG_CNT/indices exacts. Aucun temps de
poll ni grille26667 attribué aux XYZ. Delta ancre accepté23..25ticks (24observé,
allowance1tick), check compteur/DWT toutes100ms et arrêt au-delà100us de la
bracket ; enveloppe expérimentale, aucune garantie thermique/WCET. Sous-groupe
terminal non bracketé abandonné au STOP, aucune fausse fenêtre complète. Reset
réinitialise le provider ; compteur capteur étendu modulo32 avec demi-domaine,
recul/overflow/refus signalés, UTC toujoursfalse. Premierbuffer après TS0,
aucune extrapolation initiale ; résolution B3 finale1ms.

FRAM complètea13b inchangée après diagnostic. Suite préparée trigger11 : un vrai
START4/5 réservera run5, seul nouvel ID nécessaire pour B3 causal, puis STOP4/6.
Aucune isolation de journal ni renouvellementepoch. Autorité/ownership établies
par snapshot exact et recovery R2, pas par un booléen remplaçant les gardes ;
B5 reste inchangé. Charge bornée FRAM32octets puis service FIFO+publication+
RTUloopbackRAM ; vérification des zones protégées. N4096 physique prévu, autresN
restent logiciels. Conditions1000/2500/5000us strictes ; interruption de l'essai
sur violation, conservation des nouveaux identifiants, restauration FlashM016.
