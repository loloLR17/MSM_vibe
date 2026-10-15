# TR2 LIVE — profil successeur R0

Statut : **SPECIFIED_NOT_IMPLEMENTED**, version documentaire 1 du 15/10/2026. Mission `TR2-20261015-R0-NORME-LIVE-SUCCESSEUR`, HEAD examiné `2b9c0202f79bc132b166776c4aed2a48925593dc`. Ce document finalise techniquement le contrat R0 adopté. Le contrôle indépendant ChatGPT reste requis avant R1 ; ce n'est ni une modification des gels V1/V1.1 ni une qualification.

## 1. Portée et héritage

**R0-BASE.** La supervision LIVE fonctionne sans média de campagnes, sans durée limite nominale et sans création de campagne. La FRAM reste autorité de configuration, d'identité et de commandes. Les campagnes sont conservées pour R7, avec commandes 3/4 et exigences de stockage historiques. Aucune promesse de XYZ durable, de store-and-forward ou de données récupérables pendant une déconnexion PC en LIVE.

Le profil hérite de `../charte_typage.md`, des blocs 0..7 pour les champs non explicitement spécialisés ici, et **du contrat transactionnel B5 V1.1** `../V1_1/bloc5_v1_1_transaction_epoch.md` intégralement (y compris TRANSACTION-04/05/06). Les différences ci-dessous s'appliquent seulement au profil protocole 2. Aucun client V1 ne doit lui envoyer des commandes comme s'il s'agissait du protocole 1. L'implémentation courante V1 n'implémente pas déjà l'epoch : son intégration est une dépendance explicite de R2, pas un acquis.

Les tailles B0/B1/B2/B3/B4/B6/B7 restent 21/20/16/48/176/64/16 registres ; B5 reprend les 29 registres V1.1,5000..5028. Les adresses, noms, types, accès et l'ordre MSW/LSW hérités ne changent pas. Tous les réservés restent 0, sauf allocations de domaines explicitement ci-dessous. Le mapping complet compagnon fait foi comme dérivé de cette norme ; aucune nouvelle adresse.

## 2. Réservations numériques

**R0-VER.** Valeurs attribuées au successeur :

| Champ / domaine | Valeur normative LIVE |
|---|---|
| B0 protocol_version (6) | 2 / 0x0002 ; version scalaire, pas un encodage sémantique majeur/mineur |
| B0 device_capabilities (7) | bit4=LIVE_WITHOUT_CAMPAIGN_STORAGE ; bits0..3 hérités, bits5..15=0 |
| B4 config_structure_version (4000) | 2 |
| B4 config_capabilities_mask (4001) | bit0=validation LIVE ; bit1=validation campagne ; bits2..15=0 ; LIVE obligatoire bit0=1 |
| B6 inventory_structure_version (6000) | 2 |
| B7 diag_structure_version (7000) | 1 ; aucune nouvelle adresse ni nouveau bit B7 |
| B5 codes | 12=RENEW_TRANSACTION_EPOCH hérité ; 13=START_LIVE ; 14=STOP_LIVE ;15..65535 réservés |
| B6 storage_health_status (6009) | 4=UNAVAILABLE / inventaire ou capacité non caractérisés ;0..3 hérités |
| Record configuration / identité | formats 2 / 2 dans l'espace successeur ; anciens formats 1 en lecture explicite seulement |

La V1 de production est protocole 1. La V1.1 fige B5 sans réserver de valeur numérique B0 distincte dans ses textes ; protocole 2 n'est pas une réattribution à V1.1, mais le nouveau profil composé LIVE+B5 epoch. Les valeurs arbitraires 7/8 des fixtures d'identité ne sont pas des allocations produit. `reservations.json` et le contrôle documentaire référencent ces espaces séparément (commandes, résultats, versions, bits).

La V1.1 historique ne possède pas ici de produit annoncé par une valeur B0 autonome : le nouveau PC ne doit jamais déduire son support d'epoch du seul nombre1. Les deux profils produit identifiables sont V1/protocole1 et LIVE/protocole2 ; un éventuel produit V1.1 autonome demanderait sa propre annonce explicite. Cela ne modifie ni son gel B5 ni l'héritage explicite de ce profil.

Une image LIVE qui fournit réellement acquisition+supervision+diagnostic et aucune fonction d'enregistrement SD annonce B0 capabilities=0x001D. Le bit1 reste 0 jusqu'à support SD réellement implémenté ; insertion d'une carte ne le change pas. Aucune de ces valeurs ne signifie « qualifié physiquement ». B4 capabilities=1 pour une composition LIVE seule,3 si la validation campagne est aussi implémentée ; ces capacités sont statiques pendant le boot.

## 3. Configuration et CRC

**R0-CFG.** Les seules combinaisons autorisées :

| Profil | acquisition_mode | storage_mode | storage_limit_mb | campaign_duration_s |
|---|---:|---:|---:|---:|
| LIVE continu |2|0|0|0 exactement|
| Campagne |1|1|1..C réel|60..604800|

La valeur 0 de durée est une **nouvelle signification versionnée : non applicable à LIVE**, aucun minuteur de campagne ne doit arrêter LIVE. Une durée LIVE 60,3600,604800 ou toute autre valeur non nulle est refusée, pas ignorée. Le domaine V1 reste inchangé. Tout tuple mixte, acquisition 0 ou 3..65535, stockage 2..65535, limite non nulle en LIVE est refusé. La validation campagne exige en outre config_capabilities_mask.bit1=1 ; sinon elle est refusée comme profil non supporté (résultat 4). La capacité C n'est interrogée que pour campagne ; ne jamais substituer UINT32_MAX à une capacité opérationnelle inconnue.

IDs configuration/contexte/mission non nuls, labels 32 ASCII et domaines fréquence 26667, axes 1..7, pleine échelle 0..3, fenêtres 4096/8192/16384/32768, périodes 2000/5000/10000/30000/60000ms restent. Contrainte entière `period_ms*26667 >= 1000*window_samples`. Une fenêtre conserve une seule génération/échelle/axes effectivement appliqués au capteur.

**R0-ALCFG.** Pour LIVE : supervision_enable_mask=1 (bit0 active global et axes sélectionnés ; autres bits réservés 0). Seuils uint16 en mg, warning<=alarm séparément RMS et peak ; hystérésis 0..65535, hold 0..65535ms. L'égalité warning=alarm est permise, le seuil 0 aussi ; hystérésis soustraite avec plancher 0, pas de sous-dépassement entier. Les anciennes configurations campagne ne sont pas silencieusement réinterprétées : les règles LIVE ne leur sont pas appliquées.

**R0-CRC.** CRC-32 IEEE inchangé : init/xorFFFFFFFF, réfléchi EDB88320, ordre ascendant des registres, octet fort puis faible, U32 MSW/LSW, ASCII et réservés inclus. Préparé 4016..4099=168 octets ; actif 4100..4175=152 octets. Version 4000 et IDs 4002..4005 hors ces périmètres : vérifier schéma/identité séparément, même si CRC conforme. Le vecteur historique 0x5207CCFC reste intact. `vectors.json` donne les tableaux complets et nouveaux CRC LIVE ; aucune constante calculée n'est appelée validation firmware.

Les modifications préparées invalident VALIDE sans changer l'active ni B3. APPLY interdit en acquisition, revalide le CRC et les domaines au moment de l'application ; commit FRAM avant publication atomique de l'active/miroirs/CRC/révision. Échec : ancienne active intacte. config_state 0/1/2/4/5/6 hérités ; config_error_code successeur :0 aucun,1 incomplet,2 domaine/tuple,3 CRC,4 version,5 persistance. La version de configuration lève l'ambiguïté de cette nouvelle table, sans rétroattribution V1.

## 4. Cycle LIVE et états

**R0-RUN.** Machine : BOOT → NO_CONFIG ou READY → STARTING → LIVE → STOPPING → READY. Une faute bloquante conduit à FAULT ; reset physique/logiciel conduit toujours à BOOT, capteur arrêté, pas à LIVE. STARTING et STOPPING sont des états internes, sans nouvelle valeur enum B1 : acquisition_state 1 seulement après confirmation matérielle de démarrage,0 après arrêt confirmé,3 si état capteur indéterminé/erreur ; valeur 2 pause n'est jamais utilisée par LIVE. READY indique possibilité d'opérations normales de l'état, pas existence d'une SD.

START_LIVE n'alloue pas de campaign_id, n'appelle ni open/begin/append/checkpoint/finish du stockage de campagnes, ni une lecture SD de découverte. Aucun autotest SD périodique ou scan de recovery média ne bloque le boot/live. Un seul propriétaire FIFO/SPI ; une réinsertion média n'ouvre rien. Un état physique capteur non confirmé après erreur exige arrêt sûr, jamais une réponse succès déduite d'un simple write SPI.

| État / événement | B1 acquisition / CONFIG_VALID / READY | B3 | B6 | B7 / action |
|---|---|---|---|---|
| Boot non fini |0/0/0|statut 0, aucun résultat valide|santé 4, sélection invalide|santé 2, pas de faute fabriquée ; aucune nouvelle commande métier|
| Pas de configuration autoritative |0/0/1 si autorités commande disponibles|statut 1, CONFIG_VALID0|état réel/4|START refuse 22 ; APPLY possible après préparation|
| Prêt LIVE valide |0/1/1|statut 1 avant premier résultat ; ACQ_STOPPED1|état réel/4 ; ID campagne 0|absence SD prévue sans faute|
| LIVE première fenêtre |1/1/1|statut 2, pas de valeur valide avant fenêtre complète|inchangé|calcul en cours, SD non requise|
| LIVE résultat exact récent |1/1/1|statut 3, ou4 si temps civil indisponible ; qualité selon §6|inchangé|aucune faute SD pour absence prévue|
| STOP confirmé sans défaut bloquant |0/1/1|statut 1, dernière valeur conservée qualifiée, FRESH0/ACQ_STOPPED1|inchangé|pas de flush/fichier/campagne LIVE|
| SD absente volontairement |inchangé|inchangé|4 si inventaire non autoritatif|B1 storage_status0, AVAILABLE0, pas de faute SD créée|
| Panne SD réellement détectée |LIVE inchangé ; campagne selon ses garde-fous|LIVE inchangé si fenêtre exacte|santé 2 ou3 si inventaire connu, sinon4|B1 fault bit2/B7 fault bit3, santé dégradée ; cause non masquée|
| FRAM/identité/autorité commande défaillante |arrêt sûr, READY0, CONFIG_VALID0 si autorité perdue|statut 5, VALUES_VALID0|4 si autorité inventaire perdue|faute bloquante ; aucune active de remplacement|
| Configuration absente/invalide seule |0/0/1 si FRAM/identité/epoch sains|statut 1 ou5 si perte en cours|pas de fausse campagne|CONFIG_FAULT si corruption/échec réel, pas pour absence initiale normale|
| Capteur, FIFO perte, erreur calcul |arrêt si défaut bloquant ; acquisition 3 si erreur|fenêtre touchée invalide, CALC_ERROR seulement si calcul échoué|inchangé|défaut capteur/acquisition/interne, pas un résultat 0 valide|
| Reboot/coupure/retrait-retour média |toujours acquisition 0 au boot ; active seulement après recovery|snapshot live non restauré comme mesure courante|historique durable uniquement|recovery B5 sans redispatch ; pas d'auto-START|

**R0-FAULT.** Causes bloquantes LIVE : perte d'autorité FRAM/identité/epoch, erreur capteur, perte FIFO, calcul débordé, erreur interne affectant la mesure, danger alimentation/température réellement détecté. SD seule ou absence d'heure UTC ne bloquent pas le calcul numérique LIVE. Le défaut de temps est exposé avec qualité dégradée ; une simple UTC non synchronisée n'est pas un TIME_FAULT matériel.

B1 system_status :0 initialisation ;3 faute bloquant le service demandé ;2 dégradation/avertissement ou faute SD non bloquante LIVE ;1 sinon. B7 health :2 en initialisation ;3 si faute bloquante ;2 si faute non bloquante ;1 si avertissement seul ;0 sinon. B1 READY0 si initialisation, maintenance empêchant START ou autorité commande indisponible ; sinon 1 pour les opérations compatibles (y compris STOP durant LIVE). Les bits acquisition/config/temps décrivent leurs autorités réelles ; stockage disponible est toujours la disponibilité réelle du **média de campagnes**, jamais 1 par absence de besoin.

Défauts B1/B7 : capteur→B1 bit0/B7 bit0 ; acquisition→B1 bit1/B7 bit1 ; SD→B1 bit2/B7 bit3 ; horloge matérielle→B1 bit3/B7 bit4 ; configuration→B1 bit4/B7 bit5 ; FRAM→B1 bits4+5/B7 bits2+5 ; interne→B1 bit5/B7 bit6 ; surconsommation/température/interconnexion→B7 bits7/8/9 et B1 bit5 si bloquants. Tous les bits applicables s'additionnent.

B1 error_code et B7 last_fault_code ont ici le namespace commun :0 aucun,1 config,2 capteur,3 acquisition,4 FRAM/identité/epoch,5 SD,6 interne,7 horloge,8 alimentation,9 température,10 interconnexion. Si plusieurs causes apparaissent au même instant, priorité 4,8,9,10,6,2,3,1,5,7. B1 error représente le défaut actif prioritaire ; B7 last_fault mémorise le dernier événement défaut (timestamp fiable sinon 0), sans le réinventer au boot. B1 warning_code :0 aucun,1 stockage,2 temps,3 température, priorité dans cet ordre ; warning_flags correspondants conservés. Un changement de cause produit un événement, pas chaque lecture. Toute cause encore présente reste signalée malgré ACK. Guérison supprime les flags actifs, pas le dernier défaut mémorisé. ACK cible un défaut par ce code ; ACK global suit B5 param2=1. Pas de commande d'effacement des données créée.

## 5. B5, priorités et recovery

**R0-CMD.** Modèle epoch/id, mailbox atomique et ordre V1.1 intégralement conservés : accès → capture → epoch → txid/couple commande → lookup/retry/collision → code → paramètres/confirmation → contexte → admission/RESERVED durable. Résultats 23..29 et status 9 sont ceux de V1.1, jamais réassignés. Un retry exact admis est servi **avant** revalidation de l'état actuel ; même identité et requête différente→5/27. Une commande inconnue 15..65535→7/1. Un refus avant admission n'est pas une transaction passée à exécuter plus tard.

Pour 13/14 : param1/2/3=0, confirm_key=0 ; sinon 5/2. ID 1..65534 seulement. Une autre transaction non terminale→5/13, maintenance→8/18 pour START. Après ces contrôles, appliquer la première ligne applicable :

| Commande | Priorité métier, première condition vraie | Réponse / effet |
|---|---|---|
|13 START_LIVE|faute bloquante|5/8 ; aucun démarrage|
|13|absence d'active autoritative|5/22|
|13|active de mode campagne ou état non prêt/acquisition déjà active|5/3|
|13|budget mémoire/IO journal insuffisant|5/7 ; aucun STARTED/effet|
|13|sinon, sans condition SD/UTC|admission durable, STARTED puis démarrage et reçu d'effet ; COMPLETED 4/0 seulement après preuve démarrage|
|14 STOP_LIVE|campagne active|5/3 ; jamais arrêt implicite de campagne|
|14|LIVE inactive|5/21 ; pas de faux succès nouveau|
|14|LIVE active mais budget journal/IO indisponible|5/7 ; refus avant effet de commande. Une panne d’autorité réelle impose séparément l’arrêt de sécurité. Le scheduler doit garantir un chemin STOP sans famine, à démontrer en R4.|
|14|LIVE active avec budget disponible, même si défaut de mesure|admission/STARTED, arrêt sûr confirmé ; reçu puis COMPLETED 4/0 ; panne NVM empêche succès durable, arrêt de sécurité indépendant obligatoire|
|1 APPLY|acquisition active|5/5, avant toute I/O de configuration|
|1|préparation incomplète / CRC ou domaine invalide / schéma inconnu|5/20 puis 5/4 selon première cause dans cet ordre ; pas d'active partielle|
|1|échec commit|6/8 si autorité perdue,6/10 si timeout ; config_state 6/code 5|
|1|sinon|4/0 après commit et projection cohérente|
|3 START_CAMPAIGN|faute bloquante campagne / aucun active / tuple LIVE / acquisition active / SD absente / SD plein ou mémoire insuffisante|5/8,5/22,5/3,5/3,5/6,5/7 respectivement ; cas nominal ouvre campagne conformément V1|
|4 STOP_CAMPAIGN|LIVE active / aucune campagne active / sinon|5/3 ;5/21 ; arrêt/vidage/clôture V1|
|12 RENEW|autre transaction non terminale / epoch FFFFFFFF / sinon|5/26 ;5/26 ; transition explicite N→N+1 V1.1, pas de nouvel effet LIVE|

Le tableau `02_Validation/LIVE_R0/decisions_start_stop.csv` développe les 384 combinaisons booléennes de contexte pour13/14 après contrôles d'accès/epoch/identité/paramètres. ADMIT_PENDING_EFFECT signifie admissible, pas succès déjà observé. Les états transitoires contradictoires sont traités de façon conservative ; ce tableau ne remplace pas la preuve d'effet ni les priorités V1.1.

Si l'activation N+1 d'un renouvellement **déjà admis** ne peut être ni prouvée ni exclue : epoch_status5 INDETERMINATE/current_epoch0 et cmd_status9 avec identité active conservée, aucune nouvelle admission. Ne jamais remplacer ce recovery par un refus5/26, réservé aux préconditions d'un nouveau renouvellement.

Une faute SD détectée bloque la campagne mais n'entre pas dans « faute bloquante » du 13. STOP ne requiert ni UTC valide ni SD ni nouvelle configuration. Autres commandes 0..11 : sémantique héritée ; SELFTEST standard en mode LIVE n'accède pas SD, absence SD attendue n'échoue pas ; tests FRAM/capteur/temps réellement réalisés sont exposés et tout sous-test non effectué reste non démontré. REFRESH ne démarre ni acquisition ni campagne, ne rajeunit pas un snapshot sans nouveau calcul ; ENTERMAINTENANCE et reset 10 exigent acquisition arrêtée comme V1. Confirmation A55A reste exigée pour 10/11.

L'ACK spécifique du latch vibratoire utilise param1=0x0100,param2=0 ; param2=1 global peut aussi l'acquitter. Refus 16 si une alarme active subsiste ou si aucune fenêtre courante valide/fraîche ne permet de prouver sa disparition. Les autres paramètres non employés=0. Un acquittement n'est pas une suppression de la cause ni un recalcul.

**R0-REC.** Le succès B5 est historique : après reboot, un retry de START_LIVE terminal réussi restitue ce succès **sans redémarrer**. Le PC vérifie B1 acquisition et B3 qualité, pas le seul last. Une commande 13/14 durablement RESERVED sans STARTED, avec barrière prouvant qu'aucun effet n'a pu commencer→6/28. STARTED et preuve positive d'absence→6/29 ; reçu durable d'effet observé et relié à la transaction→finalisation démontrée ; sinon 9 RECOVERY_INDETERMINATE sans redispatch. Un reset n'est pas une preuve d'absence d'effet antérieur. La coupure entre STARTED et reçu peut donc bloquer les nouvelles commandes : diagnostic et résolution autoritative distincte requis, jamais un nouvelle epoch implicite ni un 28/29 inventé. L'arrêt de sécurité au boot demeure obligatoire même si le journal est indéterminé.

L'identité `(epoch,txid)` reste consommée durant l'epoch, sans GC opportuniste des commandes connues. Renouvellement explicite possible avant 65534 si budget journal atteint, uniquement sans transaction non terminale et après ordonnanceur IO sûr ; les refus ont zéro effet. Au plafond epoch FFFFFFFF : renouvellement refus 26, jamais wrap ; l'acquisition LIVE déjà en cours continue. Le futur firmware doit réserver le contrôle STOP et le renouvellement avant saturation, selon le plan FRAM compagnon.

## 6. Mesures, qualité, alarmes, temps et compteurs

**R0-QUAL.** Fenêtres de N échantillons contigus, axes et échelle appliqués issus d'une seule active. Un trou/OVR/saturation/incohérence de configuration/débordement rend la fenêtre impropre à VALUES_VALID ; les valeurs éventuellement conservées ne sont jamais présentées comme une nouvelle mesure valide. Algorithmes mg : RMSaxe=`floor(sqrt(floor(sum(x²)/N)))`, RMSglobal même règle sur somme des carrés des axes activés ; peakaxe=max(abs(x)), peakglobal=floor(sqrt(max(sum_axes(x²)))). Axes désactivés exposés 0, exclus des calculs globaux/alarmes. Pas de soustraction DC implicite ni conversion mm/s. Les arrondis sont désormais explicites pour ce profil ; ils ne modifient aucun ancien oracle V1.

Une fenêtre numérique valide a V=1 si Nvalid=N, complète, sans saturation/trou/débordement et liée à l'active. UTC n'est pas nécessaire à V. À chaque lecture, âge monotone 64 bits depuis fin de fenêtre, exposé saturé à FFFFFFFF ; F=V et acquisition active et âge<=2*indicator_period_ms. UTC disponible fiable→timestamp réel et TIMEBASE_VALID 1 ; sinon timestamp 0 et TIMEBASE_VALID 0. Un recalage UTC ne change pas rétroactivement le timestamp de la fenêtre ; seules les nouvelles fenêtres prennent la nouvelle base. Un recul ou débordement du monotone est un défaut interne, invalidation et arrêt sûr.

B3 flags : bit0 enabled=1 pour active LIVE ; bit1=V ; bit2=F ; bit3 fenêtre complète ; bit4 Nvalid=N ; bit5 pas de saturation ; bit6 UTC fiable de cette fenêtre ; bit7 configuration autoritative cohérente avec résultat ; **bit8 campagne durable active=0 en LIVE** ; bit9 dernière valeur retenue sans nouvelle fenêtre valide, notamment arrêt/erreur/périmé ; bit10 dégradé si résultat retenu/non frais ou UTC invalide ou fenêtre impropre ; bit11 erreur de calcul réelle ; bit12 acquisition arrêtée ;13..15=0. Les bits de fenêtre qualifient la fenêtre exposée ; après perte de configuration bit7/VALUES_VALID sont retirés. Au boot aucune fenêtre, bits fenêtre/V/F/TIMEBASE=0 ; un zéro numérique sans V ne vaut pas mesure.

B3 statut :0 boot ;1 inactif sans défaut bloquant ;2 actif sans première valeur ni erreur ;5 erreur de fenêtre/calcul/configuration ;4 valeur retenue/périmée ou UTC invalide ;3 sinon. Cet ordre résout les recouvrements : boot > erreur > inactif > attente > dégradé > valide. Une ancienne fenêtre ne reste pas VALUES_VALID après une fenêtre invalide ; ses nombres peuvent être gardés avec HELD1/V0. Une simple valeur récente arrêtée est HELD1/F0, et statut 1. Une fenêtre rejetée ne remplace ni les nombres ni les métadonnées de la dernière fenêtre conservée ; les états courants retirent V/F et marquent HELD. La validité ne dépend pas du statut stockage.

**R0-ALARM.** Sur chaque fenêtre V=1/F=1, pour chaque métrique globale et axe activé : warning si RMS>seuil warning OU peak>seuil warning ; état warning se retire si les deux métriques<=max(0,seuil warning-H). Alarme candidate si RMS>seuil alarm OU peak>seuil alarm ; timer monotone commence à la première fenêtre candidate, déclenche après durée>=hold (hold 0 : immédiat). Annuler le timer si le candidat disparaît avant déclenchement ou s'il manque une fenêtre valide ; aucune extrapolation pendant un trou. Alarme active se retire lorsque RMS ET peak<=leurs seuils alarme-H à plancher 0. L'égalité au seuil ne déclenche pas, sauf maintien dans la bande d'hystérésis d'un état déjà actif.

B3_EXCEEDmetric et flags4..7 reflètent l'état warning OU alarme de la métrique ; alarm flags 0..3 reflètent uniquement ses alarmes actives. Latch se pose sur toute montée d'alarme et ne disparaît que par ACK admissible ; drapeau 8 le reflète. EXCEED_COUNT s'incrémente une fois par fenêtre où l'union des dépassements passe 0→1 ; ALARM_COUNT une fois lorsque l'union des alarmes passe 0→1. On ne compte pas séparément les axes simultanés. Pendant invalidité/arrêt, conserver les états internes et drapeaux du dernier calcul avec HELD et qualité non courante, ne pas les effacer comme si la cause avait disparu ; timers en attente annulés. Reprise valide réévalue depuis ces états. Sévérité :0 si V/F non exploitables ;4 si alarme active ;3 si dépassement ;2 si UTC invalide ;1 sinon. Pas d'attribution automatique de sévérité 5 critique à un seuil vibratoire. Axe dominant : RMSmax parmi axes activés ; égalité→4, aucun résultat valide→0.

**R0-TIME.** L'ordonnancement utilise temps monotone 64 bits ; uptime B1/B7, âge, séquence et compteurs 32 bits **saturent** à FFFFFFFF et ne stoppent pas LIVE. B3_CALC_SEQUENCE démarre 1 après première fenêtre validée et n'avance qu'à une nouvelle fenêtre valide ; continue entre STOP/START du même boot, saturée sans wrap ; reboot remet0 jusqu'à première fenêtre. La saturation ne signifie pas absence de calcul : le PC doit utiliser âge/qualité et archiver les observations, sans garantir une déduplication exacte par séquence saturée. Les compteurs d'alarme/latch sont runtime depuis boot, non restaurés comme alarme courante ; le dernier défaut B7 durable est distinct. Un compteur persistant de génération config U32 ou image/run U64 ne wrappe jamais : nouvelle activation/démarrage refusé au plafond, LIVE déjà actif n'a pas de limite de campagne. La commande 11 ne remet à zéro ni CALC_SEQUENCE ni les compteurs d'alarmes/excès de ce profil ; ils restent monotones au cours du boot. Aucune opération d'entretien automatique ou reboot pour « remettre les compteurs à zéro ».

UTC uint32 ne wrappe pas : à dépassement du domaine, UTC devient indisponible, TIMEBASE_VALID 0/timestamp 0 pour fenêtres suivantes, warning temps ; calcul et ordonnancement continuent. On ne sature pas la date en prétendant qu'elle est exacte. L'epoch transactionnelle peut être renouvelée indépendamment d'une session de mesure ; son renouvellement n'est ni un reboot ni une nouvelle campagne.

## 7. Inventaire et PC

**R0-INV.** LIVE ne modifie pas le compteur/ID/historique campagne. Si l'inventaire a une autorité récupérée, exposer ses métadonnées historiques réellement connues, même si média absent ; les capacités inconnues ou l'autorité inventaire absente imposent santé 4. Sous 4 : total/validcount, used/free=0 mais **non interprétables**, selected_campaign_valid 0, tous champs d'entrée 0, aucun reset de l'historique interne. Sélection RW acceptée selon accès mais reste invalide sous 4. Un inventaire réellement vide et caractérisé a count 0/santé 0..3, différent de 4. STORAGE_AVAILABLE B1=0 quand aucun média utilisable. Réintégration après qualification R7 : explicitement recaractériser et vérifier identité média avant toute campagne ; pas d'auto-enregistrement.

**R0-PC.** Ancien PC1 rejette protocole 2 par comparaison exacte, sans écriture ; nouveau PC choisit un profil explicite, pas `version>=1`, avant lecture B5 longue ou écriture. Il lit 2/structures2/cap LIVE pour LIVE et conserve les décodeurs historiques. B5 epoch doit être supporté avant commandes du protocole 2 ; un client lecture seule peut lire B0/B1/B3 si son décodeur connaît protocole 2. Lecture partielle valide conserve règle Modbus.

SQLite garde chaque observation avec DeviceId, UTC réception PC, qualité, version/profil/structures, configuration connue ou inconnue, mode, séquence et contexte de session PC ; B2 UTC qualifiée distincte. Les anciennes lignes sans ces métadonnées restent inconnues, pas rétroattribuées. Une session PC n'est pas un boot id STM32 prouvé : reconnecter ouvre un nouveau segment, les ambiguïtés sont conservées. Aucune identité de campagne pour LIVE. Au compteur saturé/reboot/perte d’échantillons, conserver observations et trous explicitement sans promettre exactly-once. Erreur SQLite signalée, pas transformée en archive réussie ni blocage permanent du polling ; Web affiche invalidité/fraîcheur et source réelle. Les tests PC ne sont pas des preuves de mesures physiques.

## 8. Conditions d'entrée dans l'implémentation

R1 requiert revue indépendante ChatGPT de cette norme, du mapping, du plan FRAM et de la matrice ; permissions de sa propre mission. Aucun champ « à choisir » ne reste dans l'encodage LIVE de cette version. Le déclenchement physique, la disponibilité réelle de la zone FRAM, les performances et les budgets R4 restent à vérifier, **pas des contradictions normatives ni des validations acquises**. R0 ne permet aucune écriture matérielle. Les gels historiques et la proposition A06 bornée en durée sont conservés : le présent contrat continu la remplace pour le successeur seulement.
