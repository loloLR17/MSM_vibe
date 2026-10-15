# R3-B — métrologie LIVE et B3, livraison logicielle

Référence normative : LIVE_R0/PROFIL_LIVE §3/6, héritage blocs3/4 et charte.
Ni ces textes ni leurs vecteurs ne sont modifiés. Production reste V1. R3-B
ne choisit pas une composition protocole2 et ne qualifie pas le matériel.

## Oracles et frontières

| Domaine R0 | Implémentation et preuve |
|---|---|
|26667Hz, axes1..7, FS0..3, N4096/8192/16384/32768, P2000/5000/10000/30000/60000ms, P*26667>=1000*N|Validateur LIVE R1 réutilisé avant START ; active copiée et liée run/id/génération, aucun DataStore/SD/campaign_id.|
|Unités mg sans retrait DC, axes masqués avant sommes|Conversion V1 IIS3DWB extraite identique :61/122/244/488µg/LSB, mg le plus proche, égalité à demi-mg éloignée de zéro. R0 impose les formules en mg mais ne redéfinit pas cette conversion brute héritée. Pas de flottants ST ni mm/s.|
|RMS=isqrt(sum_sq/N), crête axe=max(abs), crête globale=isqrt(max(sum_axes_sq))|Calculateur entier V1 utilisé inchangé, accumulateur vérifié extrait sans changement de corps et partagé. `generate_live_metrology_oracles.py` utilise math.isqrt et rationnels indépendants. 448 vecteurs FS×masques×N×4formes, zéro/AC/constantes/proche saturation.|
|Fenêtre N contigus|Indices XYZ exacts, causalité vérifiée à chaque échantillon ; axe désactivé zéro mais XYZ physique toujours vérifié. Saturation de tout axe, y compris non choisi, invalide et arrête (conservatisme R3-A/V1 maintenu).|
|Période et continuité|Première fenêtre au premier XYZ ; suivante au premier XYZ de temps observé >= début précédent+P. Non chevauchantes, pas de rattrapage fictif ; les XYZ inter-fenêtres sont drainés/contrôlés mais pas accumulés. N fixe, mémoire constante, pas de tampon de durée P. Cette politique d'implémentation est explicitée, pas ajoutée aux gels.|
|Temps de fenêtre|Début/fin proviennent des temps d'événements fournis, durée fin-début ; UTC de la fin seulement si fiable et uint32. Pas de datation d'une ancienne fenêtre à la lecture. Débordement UTC => indisponible, zéro avec bit temps0 ; epoch0 fiable reste possible.|
|Temps monotones|Flux temps échantillon et temps de lecture chacun64bits et sans recul, y compris entre runs ; on ne compare pas un échantillon en queue à l'instant ultérieur d'une lecture B3. Tout recul/overflow interne invalide/arrête, pas un CALC_ERROR inventé.|
|Âge/fraîcheur|Âge depuis fin, exposé saturéU32 ; F=V&&actif&&âge<=2P, frontière incluse. Lecture ne rajeunit pas, annule hold en attente si non frais.|
|STOP/reprise/invalidité|STOP conserve dernière valeur et ses métadonnées, retire F, pose HELD/STOPPED ; un run nouveau requiert sa première fenêtre complète. Erreur retire V/F sans remplacer nombres/métadonnées ; perte d'autorité retire aussi CONFIG_COHERENT. INIT reboot vide, sans auto-START.|
|Alarmes|RMS OU peak strictement>seuil pour warning/candidat, retrait RMS ET peak<=seuil-H à plancher0. Hold depuis première fenêtre candidate valide, annulé par invalidité/STOP/péremption. Actifs conservés si qualité non courante ; réévalués sur nouvelle fenêtre.|
|Compteurs/latch|Union0→1 seulement, pas somme des axes. Latch sur montée d'alarme ; ACK seulement fenêtre V/F et aucune alarme active. Séquence et compteurs saturentU32, survivent STOP/START, pas au reboot. Aucune API commande11 ne les efface.|
|B3|48registres, MSW/LSW et valeurs via projection V1 réutilisée ; statut/flags/sevérité/axes/alarmes complétés selon R0. Réservés40..47=0 ; aucun CRC B3 n'est attribué par R0, aucun registre ajouté. CRC B4 reste celui de R1, non recalculé à partir de B3.|

Les formules normatives et seuils ont été inspectés avant développement. Le
générateur de vecteurs indépendants a été matérialisé après la première rédaction
du service, avant sa première compilation/test : ordre plus tardif que demandé
par la mission pour « dresser les oracles avant développement », signalé pour
revue ; aucun résultat attendu n'a été ajusté d'après une sortie du C.

## Raccordement propriétaire et temps réel

Le binding optionnel `live_fifo_acquisition_bind_metrology` consomme uniquement
le burst entièrement validé de R3-A, puis décode ses XYZ sans reparser les guards.
START matériel confirmé précède l'ouverture du service ; STOP et erreurs du
propriétaire ferment/invalident. `bind_engine` avant boot attache la notification
R2 de défaut, distinguant perte d'autorité persistante : B3 perd V/F et bit7 en
cas de panne FRAM, indépendamment du succès du STOP. Un défaut signalé lors d'une
lecture B3 demande aussi l'arrêt sûr via callback du propriétaire.

Le fournisseur `sample_time` est **obligatoire**, borné, sans I/O bloquante. Il doit
relier chaque XYZ à des temps d'événements monotones/UTC observés et qualifiés.
Aucun fournisseur STM32 n'est inventé depuis le temps de défilement SPI, HAL tick,
ou une extrapolation26667 déclarée exacte. Le test fournit explicitement une
grille rationnelle simulée ; ce n'est pas une horloge capteur qualifiée. Le
parseur R3-A garde la progression modulo32 des TS mais ne calibre pas leur domaine
ni leur correspondance UTC. Ce raccordement reste nécessaire en R4/R6 ; sans
lui, le binding refuse, sans B3 LIVE actuel physique.

## FRAM/FIFO : coût prioritaire et stratégie R4

`test_live_io_cost` exécute les vrais stores, moteur, CRC/hash, enveloppes et
phases R2 ; seul transport RAM et capteur simulés. Mesure CPU hôte `clock()`,
compteurs octets/calls du fixture ; aucun temps MCU n'en est déduit.

Chaque phase nominale :313916octets lus,52426écrits. START/STOP/RENEW4phases :
1255664lus/209704écrits ; APPLY hors acquisition5transactions :1569580lus/262130écrits.
START phase2 arme le capteur avant sa persistance ; phase3 encore active. STOP
phases0/1 persistent avant l'effet d'arrêt. RENEW persiste tout en restant LIVE.
Le refus d'admission reste donc obligatoire sans budget démontré.

Lecture du code main.c : SPI1 FRAM SYSCLK/prescaler64 ; avec configuration160MHz,
2,5MHz calculés. Transfert sérialisé minimal des seuls octets utiles d'une phase :
(313916+52426)*8/2500000 = **1,1722944s**, hors entêtes/CRC/CPU/HAL. C'est une borne
théorique conditionnelle au clock réel, pas une mesure électrique. FIFO512words,
XYZ26667/s + TS/8 =>30000,375words/s, soit17,06645ms depuis vide ; niveau300 laisse
7,06658ms nominaux. Une phase monolithique est incompatible avec cette réserve.
Augmenter SPI ou mettre un budget booléen à vrai ne constitue pas une correction.

**Stratégie minimale proposée, pas implémentée/qualifiée ici :** garder formats
atomiques et autorité unique ; rendre le transport FRAM/CRC/hash coopératif par
quanta bornés, avec service FIFO entre quanta et avant chaque section CPU longue.
À titre de point de départ mesurable,32octets SPI à2,5MHz coûtent102,4µs utiles,
auxquels ajouter entête/HAL/CPU ; seuil d'admission dépend niveau FIFO réel,
max_gap et coût pire observé. La sélection de bank et publication atomique restent
sans réentrance. Le callback de service ne peut lancer ni commit FRAM imbriqué ni
redispatch B5 : FIFO et réception/arrêt de sécurité seulement. Capturer une demande
STOP sans attendre la fin d'un quantum ; conserver les réserves STOP126/renew127
et vérifier toutes les phases, timeout/panne inclus. Persistance d'un succès STOP
requiert toujours son journal ; l'arrêt sûr de faute reste indépendant.

R4 doit mesurer le pire temps complet d'un quantum (y compris CRC/hash), charge
SPI3, FIFO max, RX/Modbus, STOP, profondeur de pile/IRQ, et injecter défauts/coupures
sur cette version coopérative. Optimiser le format en journal delta serait une
alternative **structurante**, distincte, à arbitrer seulement si cette solution
incrémentale échoue. Pas de changement de format ni faux budget autorisé ici.

## Projections et effets : préparation, dépendances restantes

| Bloc/effet | Disponible réel / condition restante avant RTU2 |
|---|---|
|B0|Identité R1 protocole2 sans capacités prématurées ; projecteur V1 masquebit4. Ne pas simplement élargir le masque : composition opérationnelle et attribution FRAM manquent.|
|B1|Projecteur existant réutilisable pour layout20registres. Agrégateur successeur doit assembler authorities R1/R2, observation capteur, pas de campaign_id, stockage réel, défauts/READY de R0 §4. Aucune projection partielle n'est présentée comme un système complet.|
|B3|Nouveau service/projecteur complet en mémoire, testé ; aucun routage RTU ajouté. Callback temps capteur et composition temporelle restent requis.|
|B4|Projecteur R1 schema2/cap1/CRC, APPLY R2 déjà testé ; ne pas partager mailbox/active V1.|
|B5|Moteur R2 29registres/epoch, non raccordé au PDU/RTU V1 20registres ; coûts et garde admission ci-dessus.|
|B6|Projecteur V1 refuse santé4 ; successeur doit fournir schema2 et unknown4, selected_valid0/entrée0/counts0 non interprétables, sans effacer historique. Média actuel non caractérisé.|
|B7|Layout16/version1 réutilisable ; priorités R0/événements/durabilité/ACK à raccorder aux autorités réelles. Horloge civile absente seule => warning, pas faute matérielle fictive.|
|0..11 hérités|0/1 et transaction R2 présents ;2..11 restent refusés explicitement dans ce moteur. Préparer routes SYNCTIME2, refus campagnes3/4 sur LIVE, ACK6 dont0100 ou global et preuve disparition, SELFTEST5 sansSD non testée, REFRESH7 sans rajeunir B3, maintenance8/9 et reset10 seulement arrêt, clear11 sans effacer compteurs R0. La documentation normative et commande_policy existante restent autorités ; pas de réutilisation aveugle du dispatcher V1 journal20.|

Tant que les lignes ci-dessus et R4 ne sont pas satisfaites, production V1, aucun
bit LIVE ni RTU2. Cette frontière est prévue par la mission, pas un checkpoint
logiciel présenté comme une chaîne end-to-end.

## Maîtrise d'architecture

Conservé : R0/gels/tests, finiteFIFO, V1, driverST/transport, R1/R2/FRAM.
Mutualisé : accumulateur sans changement de corps, conversion brute V1 identique,
calculateur isqrt et encodage B3 existants. Ajout minimal : état LIVE persistant
runtime, alarmes/qualité spécifiques R0, callback causal/temps. Aucun module
historique supprimé ou déclaré mort. Différé : compositionRTU2, coopérationNVM,
provider temps physique, effets hérités et PC. Métriques/inventaire basés sur Git,
CMake et symboles dans les preuves ; sources compilées ≠ source liée au produit.
