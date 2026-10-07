# Gel P12 / H3h — Composition du stockage campagne de production — Tranche A

## 1. Statut et portée

**CONTRAT ARCHITECTURAL GELÉ — DOCUMENTAIRE UNIQUEMENT.**

Ce document formalise les arbitrages approuvés par le pilote pour préparer
l'utilisation du backend bulk H3h comme stockage des données de campagne du
`SystemRuntime` de production. Il ne déclare ni l'injection implémentée, ni la
composition STM32 qualifiée, ni H3h-E4 physiquement validée.

Baseline examinée : `54f4f59d2dc60c1804680881993f63e2b537a70f`.
Les lettres A/B/C/D/E ci-dessous désignent les tranches de cette composition
production ; elles ne renomment pas les sous-tranches historiques H3h-A..E.

Aucun code, test, CMake, format persistant ou mapping Modbus n'est modifié par
ce gel. Les choix explicitement différés ne sont pas des spécifications acquises.

## 2. Références et constat de composition

Références de gouvernance, dans le même répertoire :

- [Arbitrage du stockage bulk H3h-A](ARBITRAGE_FIRMWARE_P12H3H_A_CAMPAIGN_BULK_STORAGE.md) ;
- [Contrat de données campagne P8-B](FREEZE_FIRMWARE_P8B_CAMPAIGN_DATA_CONTRACT.md) ;
- [Autorité metadata D4-A](CONCEPTION_FIRMWARE_P12H3H_D4A_BULK_METADATA_AUTHORITY.md) ;
- [Layout et intégrité D4-C](CONCEPTION_FIRMWARE_P12H3H_D4C_MULTI_CAMPAIGN_BULK_LAYOUT.md) ;
- [Validation du backend D5-C](VALIDATION_FIRMWARE_P12H3H_D5C_BULK_DATA_STORE.md) ;
- [Gel du média transactionnel H3d2](FREEZE_FIRMWARE_P12H3D2_PORTABLE_TRANSACTIONAL_MEDIA.md) ;
- [Procédure physique E4](PROCEDURE_FIRMWARE_P12H3H_E4_PHYSICAL_POWERLOSS_RECOVERY.md).

Références logicielles sous `../05_Firmware/` :

- `include/tr2/application/system_runtime.h` et `src/application/system_runtime.c` ;
- `include/tr2/persistence/campaign_data_store.h` ;
- `include/tr2/persistence/campaign_data_store_persistent.h` ;
- `include/tr2/persistence/campaign_data_store_bulk.h` et son implémentation ;
- `src/application/campaign_service.c` ;
- `src/domain/configuration/configuration_validator.c`.

Le runtime actuel embarque `CampaignDataStorePersistent`, sa région média et
son core de stockage. `recover_campaigns()` initialise ce backend concret ;
`recover_campaigns()` et `compose_fg_runtime()` obtiennent son interface.
`CampaignService` utilise déjà l'interface abstraite `CampaignDataStore`.
L'évolution attendue concerne la composition, sans réécriture du contrat P8.

## 3. Décisions gelées — injection et ownership

`CampaignDataStore` devient une dépendance obligatoire injectée dans
`SystemRuntime`. Une seule instance et la même interface sont utilisées pour
le recovery et pour `CampaignService`.

La composition externe possède le backend, son média et ses buffers.
`SystemRuntime` les emprunte : il ne les alloue ni ne les libère.
La composition assure leur durée de vie pendant toute leur utilisation par
le runtime. Aucun ownership dynamique ou ambigu n'est introduit.

La composition choisit explicitement le backend : bulk pour la production,
backend simulé pour les tests appropriés, backend historique pour une
composition historique explicite. Aucun fallback automatique n'est autorisé,
notamment en cas d'absence ou d'erreur du média bulk.

`CampaignDataStorePersistent` est conservé comme backend historique explicite.
Il n'est ni cache ni fallback du bulk. Ce gel n'autorise pas son effacement,
une migration de ses données, ni un routage automatique entre deux autorités.

## 4. FRAM et microSD : responsabilités persistantes

| Autorité | Responsabilité |
|---|---|
| FRAM / persistance transactionnelle | Configuration, historique temps, repository et métadonnées applicatives des campagnes, journal commandes, diagnostic/self-test et boot intent |
| microSD bulk | Descriptors bulk A/B, états techniques OPEN/FINISHED, préfixes durables et blocs de records vibration contrôlés par CRC |
| Région historique data en FRAM | Réservation persistante conservée ; backend historique utilisable explicitement |

FRAM et microSD restent deux autorités distinctes. Aucune transaction atomique
commune n'est introduite. L'identifiant de campagne partagé ne constitue pas
à lui seul une preuve d'association entre FRAM et carte.

La région historique data en FRAM reste réservée. Aucun offset persistant
existant ne doit être déplacé. Dans la composition actuelle, cette région
précède le journal commandes puis les autorités suivantes : retirer son usage
runtime ne permet pas de supprimer sa réservation dans le layout.

Aucune migration FRAM vers microSD implicite n'est autorisée. Le présent gel
ne choisit aucune politique d'import des campagnes historiques.

## 5. Capacité : sens gelé et calcul différé

`storage_limit_mb` désigne un **plafond logique par campagne**.
Un MB signifie ici **1 000 000 octets**. Le plafond concerne les données
logiques de campagne, et non les octets physiques de headers, CRC ou padding.

Les notions suivantes restent distinctes :

| Notion | Sens |
|---|---|
| Capacité physique microSD | Capacité effectivement détectée du support |
| Région bulk réservée | Zone réellement attribuée au backend de production |
| Overhead metadata/layout | Descriptors, headers, CRC, alignements et réserves applicables |
| Capacité logique utilisable | Volume de données campagne représentable dans la géométrie et le profil retenus |
| Limite configurée | Plafond logique demandé pour une campagne par `storage_limit_mb` |
| Espace déjà consommé | Extents persistants conservés ; distinct de la limite d'une nouvelle campagne |

La capacité publiable ne doit pas être déduite de la FRAM, d'une capacité
nominale supposée, ni de la fenêtre E4. Le calcul réel, la géométrie et la
readiness appartiennent à D. Aucune valeur, taille de buffer ou formule de
publication définitive n'est figée ici. Les exigences de caractérisation de
`ConfigurationValidationEnvironment` de H3h-A restent applicables.

La réservation intégrale du quota au START et le comportement exact à
l'atteinte du quota sont explicitement différés à E. Un plafond configuré
n'est donc pas déclaré ici comme une réservation acquise ou une garantie de
place disponible jusqu'à la fin de campagne.

## 6. Format production et provisioning

Le format bulk D4-C existant est retenu comme format de production :

- 8 slots dérivés de la capacité du repository ;
- descriptors A/B ;
- extents append-only ;
- blocs contrôlés par CRC ;
- recovery du seul préfixe durable.

La sélection du format ne fixe pas la géométrie physique de production.
La fenêtre et le marqueur E4 restent exclusivement des dispositifs de
qualification et ne font pas partie du layout production.

Aucun formatage, nettoyage, réparation, rétention ou récupération d'espace
implicite au boot n'est autorisé. Une campagne OPEN après reboot n'est pas
reprise automatiquement. Une campagne terminée n'est pas implicitement
supprimée pour libérer un slot ou des extents.

Les règles existantes de D4-C restent applicables : une queue non checkpointée
n'est pas une autorité durable ; aucun octet appartenant au préfixe durable
d'une campagne antérieure ne peut être réalloué. Cela n'introduit pas une
nouvelle politique de récupération d'espace au boot.

La géométrie de production, la readiness, le provisioning explicite et
l'association FRAM <-> carte sont reportés à D. Ce document ne définit donc
ni une procédure de préparation de carte, ni un nouveau format de volume,
ni une réparation de média incompatible/corrompu, ni un traitement automatique
des incohérences entre les deux autorités.

## 7. Frontière avec acquisition, buffering et stalls

L'interface actuelle reste synchrone.

| Opération / propriété | Garantie gelée |
|---|---|
| Append réussi | Données acceptées/copiées ; pas nécessairement durables |
| Checkpoint réussi | Préfixe publié durablement |
| Finish réussi | Barrière finale durable |
| Durée des appels | Potentiellement bloquants, sans borne de latence revendiquée |
| Erreurs | Aucun retry automatique ni changement automatique de backend |
| Accès | Aucun accès concurrent/réentrant non prévu |

Le contrat P8-B reste inchangé : records explicites de 16 octets par sample
lu avec `TR2_OK`, checkpoint en fin de fenêtre avant publication supervision,
finish durable et aucune reconstruction de la queue non checkpointée.
Une erreur n'est pas transformée en preuve de durabilité ou succès de campagne.

La stratégie acquisition/buffering/stalls appartient aux tranches suivantes.
Ce gel ne définit ni FIFO/DMA IIS3DWB, ordonnanceur temps réel, gros buffer,
politique définitive de stalls, compression, FFT ou export.
La future couche doit respecter ces garanties de durabilité et prendre en
compte le caractère bloquant des appels ; aucune API asynchrone ni stratégie
d'absorption des stalls n'est imposée par cette tranche.

## 8. Décisions différées et conséquences pour B/C

| Tranche | Responsabilité future |
|---|---|
| B — Injection SystemRuntime | Raccorder la dépendance obligatoire ; employer la même instance pour recovery et CampaignService ; adapter les compositions externes ; préserver les offsets et l'ownership gelés |
| C — Tests runtime/bulk | Démontrer la composition avec backend simulé et bulk, les records/checkpoints/finish/recovery, les erreurs et l'absence de fallback ; préserver les preuves des tests historiques |
| D — Géométrie/readiness/provisioning/capacité | Définir la géométrie de production, l'inspection de readiness, le provisioning explicite, l'association FRAM <-> carte et la capacité réellement publiable |
| E — Quota/admission | Décider la réservation intégrale ou non au START et le comportement exact à l'atteinte du plafond logique |

B/C ne doivent pas introduire les politiques différées à D/E, publier une
capacité fictive ou modifier un format persistant pour faciliter les tests.
Un backend simulé est une preuve logicielle de composition, pas une preuve
matérielle ou une valeur de capacité production.

La taille des buffers, le découplage acquisition/écriture et la gestion des
stalls restent hors de cette séquence de décisions gelées ; aucune constante
issue du harness n'est promue en dimensionnement de production.

## 9. Invariants et compatibilité avec les gels existants

- Aucun changement des sémantiques ou mappings Modbus B0..B7.
- Contrat P8-B des records et frontières de durabilité conservé.
- Format bulk D4-C et statuts de recovery existants conservés.
- Autorité transactionnelle FRAM H3d2/H3d3 distincte du bulk, sans migration
  implicite ni déplacement des offsets persistants.
- Aucune fausse capacité, preuve physique ou garantie de latence déduite de
  cette formalisation documentaire.
- Aucune auto-reprise OPEN ni opération implicite de remise en état du média.

Le présent gel complète les choix de composition laissés ouverts par D5-C.
Il ne supersède pas les limites des validations physiques historiques et
n'annonce pas une intégration de production déjà réalisée.

## 10. Baseline physique H3h-E4

La qualification physique H3h-E4 reste attachée exactement au firmware :

`111ca804c08cd126abc0ae10ff2a923ce7554ebe`.

Cette baseline Git sera reconstruite exactement puis qualifiée selon la
procédure E4. Le développement continue sur `main`.
L'applicabilité de la preuve E4 au futur firmware de production sera évaluée
séparément à partir des changements et de la portée réellement qualifiée.

Le gel de ce contrat n'est ni une qualification physique E4 ni une preuve
que le runtime de production satisfait les performances d'acquisition/stockage.

## 11. Validation de la tranche A

La validation de cette tranche documentaire porte sur la correspondance avec
les arbitrages du pilote, la cohérence avec les références, l'inspection du
diff et `git diff --check`. Aucune compilation firmware n'apporte de preuve
supplémentaire pour cette modification exclusivement documentaire.
