# M014-E — autorités, provisioning explicite et boot sûr

11 octobre2026. **VALIDÉ logiciel sur média RAM / COMPILÉ ARM ; BLOCKED pour la
composition/provisioning de carte. M014 reste IN PROGRESS.** Aucun flash, debug,
reset, accès FRAM/SD réel ou COM7. Décisions de principe approuvées dans
[le document D](INTEGRATION_SUPERVISION_M014_D.md#décisions-darchitecture-validées--préparation-du-provisioning-de-production-11102026) ;
les choix d'allocation/format ci-dessous ne sont pas implicitement approuvés.

## Audit normatif et sources réelles

- `01_Specification_source/bloc0.md`, parties normatives 1 à 9 : B0=0..20,
  DeviceId **uint32** aux0/1 MSW/LSW, unique/persistant, versions2..6,
  capacités7 bits0..3 seulement, ASCII 16/8 aux8..19, réservé20=0.
  `modbus_project_b0` applique exactement ces champs. L'adresse Modbus n'a
  **aucun registre B0 normatif** ; section0.7 informative n'en ajoute pas.
- `bloc1.md` : B1=1000..1019, snapshot cohérent, system_statusUNKNOWN=0/NOMINAL=1/
  DEGRADED=2/FAULT=3 ; READY est le bit0 de system_flags, pas un état fabriqué.
  Une imageFRAMVALID n'est ni un runtimeREADY ni une sourceB1 complète.
- `IdentitySnapshot`/`IdentityService` contiennent l'identité fonctionnelle en RAM,
  sans source persistante de carte. `modbus_project_b0` ne dérive aucun DeviceId.
  `B0Identification`/`B0SessionFactory` PC attendent DeviceId 32 bits et version explicite.
  L'unicité entre appareils relève d'un registre d'attribution, pas d'un test local.
  Aucun nouveau domaine réservé de DeviceId n'est inventé par ce moteur.
- HALCubeU5v1.9.0 `stm32u5xx_hal.c:487..507`, CMSISu575 UID_BASE : trois motsUID
  32bits. Le moteur transporte/comparera les **96bits entiers** internes ; aucun
  UID réel n'a été lu, aucun hash/troncature vers DeviceId/serial ASCII n'est ajouté.
- Adresse indépendante : validateur réel `modbus_rtu_server_runtime_unit_id_is_valid`
  accepte1..247 ;0broadcast et248..255 refusés pour serveur. Cette politique
  de transport n'est pas un nouvel enregistrement B0 ou une écriture réseau.
- GéométrieFRAM262144octets (`TR2_STM32_FRAM_CAPACITY`), image logique51818,
  deux superblocks64 et deux images avec header64. Le profil *qualification* est
  fourni explicitement ; pas un profil de production implicitement approuvé.
- `system_persistent_layout.h` : tous offsets historiques conservés, BootIntent
  commence51802 et occupe16 → **fin51818, zéro réserve logique libre**.
  Le domaine historique CampaignData reste réservé même lorsque les campagnes
  utilisent le bulkSD. Il ne peut être réattribué silencieusement à l'identité.
- `TransactionalImageMedia` publication transactionnelle : payload inactif,
  headerCRC/génération, puis superblock, vérifications ; recovery sans écriture.
  `format_empty` est explicitement destructif, distinct de recovery/boot.
- `ProductionApplication` possède descripteurs/services ; les vrais backendcontexts
  sont empruntés et doivent durer. `stm32_production_binding_acquire` faible reste
  NOT_AVAILABLE ; aucun fixture/hook permissif/identité par défaut ajouté.
- `SystemRuntime`/serveur conditionnent disponibilitéB1 aux snapshots et refresh
  réels ; binding a sa propre readiness. Un boot normal complet peut écrire ses
  journaux/BootIntent (par exemple clear d'un intent récupéré), contrairement au
  probe de cette mission qui est strictement en lecture. Il exige donc ownership
  et autorisations média adaptés ; ne pas annoncer un boot complet sans écriture.
- `Stm32SdmmcBulkMedia`/`CampaignDataStoreBulk` existent, avec buffers/contextes
  durables, metadata et slots ; les instances E3/E4 du harness sont qualifications,
  pas composition exclusive production. Pas microSD utilisée comme FRAM.

## Matrice d'autorités et d'état

| Autorité | Ce qui existe réellement | Condition / reste à fournir |
|---|---|---|
| UID matériel96 | HAL three-word accessor, type interne moteur | lecture future sur cible identifiée, pas observable dans cette mission |
| DeviceId 32 bits/B0 | IdentitySnapshot/projection gelée | attribution unique explicite et stockage validé, fabricant/serial/versions/capacités réels |
| Adresse | unit validator1..247 | champ interne indépendant persisté, migration explicite et versionnée |
| FRAM transactionnelle | geometry/init/recover/commit testés | géométrie physique exclusive approuvée et image valide, aucun format implicite |
| Autorité d'inspection image | interface moteur explicite | parseur du futur layout entier approuvé ; absent actuellement → refus |
| Configuration | ConfigurationStore/workflow/validate réels | vraie configuration valide et metadataallocator durable, pas image zéro déclarée valide |
| BulkSD | SDMMC/CampaignDataStoreBulk existants | propriété/allocations/buffers durables de carte et média réellement disponible |
| Transport | chaîne diagnostic qualifiéeE | composition production + adresse/identité sourcées, pas nouvelle qualification ici |
| B0/B1 production | sources optionnelles/projections réelles | B0 connu et runtimeB1 autoritatif ; rien issu du seul succèsprobe |
| SSHHP | scriptsOFFLINE observés en SSH-HP-001 | procédure/gouvernance ci-dessous, COM7/exclusivité physique toujours non prouvés |

| État observé | Résultat du probe logiciel | Action sûre |
|---|---|---|
| Interface absente/non initialisée | NOT_AVAILABLE + recoveryUNAVAILABLE | pas inférence d'absence physique ; aucune écriture |
| Lecture FRAM inaccessible | UNAVAILABLE | pas masquage par une ancienne autoritéRAM |
| EMPTY (headers/superblocks uniformes) | NOT_AVAILABLE | création séparée seulement après choix/autorisation, pas autoformat |
| VALID | OK + génération/image | données récupérées, valider identité/config/dépendances avant services |
| Pair déchiré/corrompu avec publication valide survivante | recoveryVALID selon algorithme existant | preuveCRC/publication survivante avant disponibilité, aucune réparation physique automatique |
| Formats non supportés sans autorité valide | UNSUPPORTED | arrêt ; migration spécifique, pas interprétation approximative |
| CORRUPT/conflict/unrecoverable | CORRUPTED | sauvegarde/diagnostic, pas écriture de secours silencieuse |

**RECOVERABLE n'est pas une valeur publique du code actuel.** La récupération
existante classeVALID si une publication autoritative complète survit, y compris
un pair déchiré ; sinon états ci-dessus. Ne pas inventer une nouvelle enum ni
transformer une I/O inconnue en corruption ou en succès. ABSENT physique exige
une preuve séparée ; l'API distingue seulement absence d'interface/inaccessibilité.

## Livraison logicielle sans allocation inventée

`production_provisioning.h/.c` :

1. `probe` réutilise recovery en lecture seule et traduit ses états explicitement.
   Le préparateurSTM32 appelle désormais ce probe avant capteur/services ; gate
   VALID conservée, erreurs plus précises, aucune invocation de provisioning.
2. `plan` exige ownership explicite, génération attendue, image complète51818
   indépendante/immuable et une **autorité d'inspection de tout le layout**.
   Sans parseur réel/ownership → NOT_AVAILABLE. ValidationUID 96 bits exact/versions,
   adresse1..247, capacitésréservées refusées et projectionB0 existante.
   Identité/B0/générationfonctionnelle/adresse actuelles doivent rester identiques
   dans cette opération ; modification/migration non implicitement autorisée.
3. `apply` exige une autorisation d'écriture fournie séparément, replannifie puis
   écrit via PersistentStorageCore/TransactionalImageMedia une seule transaction,
   récupère et compare tous les octets. Aucun retry. Payload identique après
   contrôle de la génération courante → aucune nouvelle transaction. Un ancien
   manifeste avec génération périmée est refusé ; rafraîchir le plan en lecture
   avant une future opération, ne pas transformer un timeout en retry automatique.
4. EMPTY est refusé même avec flagwrite_authorized : **ce moteur ne crée pas
   l'image initiale, ne formate pas, ne provisionne pas l'identité pour la carte**.
   Son chemin mutation est utilisable seulement après un layout/parseur valide.
   Il n'est pas appelé par le boot normal. Les tests RAM démontrent son mécanisme,
   pas l'existence d'une autorité de production manquante.
5. Défaut de recovery corrigé : `recovered=false/recovery_required=true` avant
   chaque tentative ; seule récupérationVALID rétablit l'autorité. Ancien état
   ne permet plus lecture/write/commit après nouvelle panne/corruption/EMPTY.
   Contrôle négatif sur sourcepréfix reproduit le défaut ; test passe après patch.

Fixtures du nouveau test : identiténative en mémoire au début d'une imageRAM,
DeviceId76543210/unit23/protocol7 et UID trois mots arbitraires explicitement
hôte. **Pas un codec durable, pas une adresse/allocation carte, pas un parser
production**, jamais raccordés à la fonction faible de carte. Le callback de
production devra inspecter les formats approuvés entiers et vérifier les
conditions métier par les services existants ; aucune confiance par défaut.
`identity_available` décrit la sourceB0 inspectée du plan, jamais un READY/B1
ni un serveur activé. Pas accès d'écriture Modbus ajouté.

## Schéma et manifeste proposés — décision nécessaire

Identité interne candidate : UID 96 bits intégral immuable, DeviceId 32 bits explicitement
attribué, serialASCII16/manufacturerASCII8, hardware/firmware/protocol/capabilities
conformes B0 ; unité1..247 indépendante. Enveloppe persistante à choisir : magic,
version de format interne, longueur, génération, intégritéCRC, liaisonUID/DeviceId.
Versionformat != versionprotocolB0 ; aucun registre réservé consommé. Ne pas
sérialiser une structC avec padding/endianness native sur la carte.

Le manifeste de provisioning futur doit fixer : mission/code/toolSHA, imageSHA,
UIDcible complet, attributionDeviceId unique et identitéB0, adresse, toutesversions,
géométrie/rangesFRAM/SD exclusives, état/génération attendus, préconditionsconfig,
imagesavant/backups vérifiés, nombre/plage des écritures et politique des coupures,
autorisation spécifique et relecture/recovery après redémarrage. Les objetsRequest
livrés contrôlent un **sous-ensemble en mémoire** de ce manifeste, pas une procédure
opérateur complète ni une autorisation sur média réel.

Options structurantes (aucune retenue silencieusement) :

- Réservation persistante séparée dans FRAM physique hors les quatre régions
  actuelles, format/version/CRC et protocole de liaison atomique à l'image existante.
  Des octets physiques peuvent être libres dans le profil de qualification ;
  cela ne prouve pas ownership ni sauvegarde de la carte. Deux domaines exigent
  une stratégie des coupures entre leurs publications et leur qualification.
- Évolution du layout logique/version avec migration contrôlée et extension de
  la taille/geometry. Tous services/offsets/médias existants doivent être pris
  en compte ; ne pas simplement augmenter51818 ou écraser la finBootIntent.
- Réaffectation du domaine historiqueCampaignData exige migration/ownership et
  audit des anciens chemins qui l'utilisent. Pas autorisée implicitement du
  seul fait que le bulkSD est choisi ; risque de perdre les données existantes.

L'adresse peut devenir configurable via procédure interne versionnée approuvée,
indépendante de DeviceId. Le moteur actuel **refuse** une modification d'identité/
adresse ; il ne prétend pas fournir déjà cette migration. Même qualification pour
mises à jour des versions/champsB0 : elles ne sont pas acceptées silencieusement.

Le diagnostic existant est conservé : imagequalifiéeE restaurable, FC03/1000/1
UNKNOWN=0. Faute B0 fiable/runtime réel, ne pas ajouter un diagnosticcomplet faux
ou overridepermissif. Un mode dégradéproduction avec B0/B1 requiert de valider les
sources/snapshots réellement disponibles et d'isoler toutes les écritures runtime.
L'absenceSD ne prouve pas automatiquement que tout le runtime peut continuer.

## Validation, reprise et qualification suivante

- Validationcomplètefirmware :111/111 CTest (un nouvel exécutable avec plusieurs
  scénarios), cross-buildSTM32 réussi ; warningsnewlib habituels4 conservés.
- Tests : média/interfaceabsent, EMPTY, VALID, peerrecoverable, lecturesinaccessibles,
  corruption/unsupported, révocationlecture/write/commit, projectionB0DeviceIdexact,
  ownership/parserabsents, générationpérimée, mismatchtroisièmemotUID, protocol,
  adresse0/248, DeviceIdchangé/capacitésréservées, bufferalias, autorisationabsente,
  idempotence, commit/relecture/rebootRAM et trois points de coupure partielle.
  Tous111tests existants/nouveau exécutés, incluant configurationfaultinjection,
  systemruntimebulk et testsRTUB0/B1/disponibilité. Aucun résultat physique.
- Six tests SSH Python réussis ; aucun changement.NET → pas relance de524tests
  ni republicationpaquet. RésultatsD conservés et identifiés historiques.
- Preuves `/home/lolo/dev/msm/reports/TR2-mission014-E-20261011T212010Z`.
  BIN/ELFdiagnosticE rehashés exacts sur disque, pasnouvellelectureMCU.

Reprise : choisir et approuver le domaineidentité/format/lienatomique avecconfig,
allocationSD/metadata/capacités/profilréel et attributionDeviceId/address/version ;
implémenter codec/inspecteur réel et tests de migration/EMPTY/création et coupures
multi-domaines. Fournir ensuite des autoritéscartedurables, bootexclusif sans
harness destructif, tests de dépendances absentes avant un flash autorisé ultérieur.
Aucune autorisation physique héritée de cette mission. Seulement après sauvegardes/
procédure spécifique/revueChatGPT : provisioning réel +persistance après reset,
imageidentifiée et B0/B1bornés via HP, puis polling/perte-reprise. C/I consommés,
aucun retry ni halt pendant trafic. M014 et les réserves timing/robustesse/E4 restent
ouverts ; BLOCKED sur décision d'allocation/format, pas sur quota ni tests échoués.
