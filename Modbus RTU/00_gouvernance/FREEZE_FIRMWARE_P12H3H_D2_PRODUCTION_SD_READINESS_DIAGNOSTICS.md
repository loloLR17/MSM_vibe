# Gel P12 / H3h — D2 — Readiness et diagnostic microSD de production

## 1. Statut et références normatives

**MODÈLE CONCEPTUEL GELÉ — DOCUMENTAIRE UNIQUEMENT.**

Baseline examinée : `9e77eb78e600b1ca63e8602ba2b012468ac4f791`.
Ce document formalise les décisions D2-F approuvées. Il ne déclare pas leur
implémentation ni leur validation logicielle ou physique. Les noms d'états et
de diagnostics sont conceptuels : aucun ABI, enum C, structure C, signature
API, registre ou mapping Modbus n'est défini ici.

Références, dans le même répertoire :

- [Composition A](FREEZE_FIRMWARE_P12H3H_PRODUCTION_CAMPAIGN_STORAGE_COMPOSITION_A.md) ;
- [Architecture D0](FREEZE_FIRMWARE_P12H3H_D0_PRODUCTION_SD_VOLUME_READINESS.md) ;
- [Format volume D1](FREEZE_FIRMWARE_P12H3H_D1_PRODUCTION_SD_VOLUME_FORMAT.md) ;
- [Format campagne D4-C](CONCEPTION_FIRMWARE_P12H3H_D4C_MULTI_CAMPAIGN_BULK_LAYOUT.md) ;
- [Procédure E4](PROCEDURE_FIRMWARE_P12H3H_E4_PHYSICAL_POWERLOSS_RECOVERY.md).

La numérotation D2 concerne la composition production, sans renommer les
sous-tranches historiques H3h. Les gels A/D0/D1 restent applicables.

## 2. Faits existants vérifiés, distincts des décisions futures

Sources sous `../05_Firmware/` :

| Source / symbole existant | Comportement observé par lecture du code |
|---|---|
| include/tr2/persistence/campaign_bulk_media.h | Interface octet/capacité et read/write/sync ; aucune readiness volume |
| src/persistence/campaign_data_store_bulk.c, campaign_data_store_bulk_init() | Validation des arguments, buffers et capacité ; init ne valide pas tout le contenu ni un volume D1 |
| Même fichier, scan_layout() | Scan des slots, validation des préfixes, IDs dupliqués et recouvrements ; mécanisme interne, pas API globale de readiness |
| Même fichier, recover_campaign() | EMPTY pour ID non trouvé ; VALID avec préfixe durable ; erreurs média STORAGE/UNAVAILABLE classées UNAVAILABLE |
| include/tr2/persistence/campaign_data_store.h | Résultat recovery générique : statut et longueur ; pas d'état technique OPEN/FINISHED ni inventaire global |
| src/application/system_runtime.c, recover_campaigns() | Recovery des IDs présents dans le repository FRAM ; aucun appel campagne si repository vide |
| Même fichier, compose_fg_runtime() | Même interface injectée utilisée pour CampaignService et recovery |
| Même fichier, system_runtime_boot() | Erreur de retour recovery interrompt le boot ; un statut données défavorable avec retour TR2_OK est conservé sans rejet direct par ce statut |
| src/application/campaign_service.c, campaign_service_start_reserved() | Ouverture repository FRAM avant begin_campaign() du backend |
| src/application/command_start_acquisition.c | Contrôles campagne/configuration, contexte transactionnel et convergence après échec ; pas encore de garde volume D2 |
| src/application/system_runtime_acquisition_command.c, collect_runtime_system_state() | B1 reçoit storage_available=true et storage_status=TR2_B1_STORAGE_AVAILABLE inconditionnellement |
| Même fichier, system_runtime_drive_acquisition_step() | Publication d'une nouvelle fenêtre de supervision conditionnée notamment au succès stockage/checkpoint |
| src/domain/configuration/configuration_validator.c | ConfigurationValidationEnvironment caractérise capacité connue et capacité utilisable ; pas association ou readiness dynamique |

Les tests `tests/integration/test_system_runtime_bulk.c` couvrent notamment
reboot logique, préfixes durables, erreurs append/checkpoint et absence de
fallback historique. Ils ne prouvent pas le futur modèle volume/readiness D2.
Le présent gel ne modifie aucun de ces comportements.

## 3. Séparation et représentation gelées

La readiness n'est pas un booléen unique. Séparer impérativement :

1. observations techniques et provenance des faits ;
2. classification diagnostique ;
3. validité temporelle de l'évaluation ;
4. décisions opérationnelles dérivées.

Retenir une structure multidimensionnelle d'observations, des diagnostics
cumulables, une cause principale dérivée et des décisions calculées depuis
un snapshot cohérent. Ne pas modifier indépendamment une décision favorable
sans faits cohérents qui la justifient. Une cause principale ne masque pas
les autres diagnostics ; un contrôle non exécuté reste non évalué.

| Dimension minimale | Informations à distinguer |
|---|---|
| Système / communication | Disponibilité de communication et autorités système |
| Média physique | Présence connue ou inconnue, init, accessibilité et capacité observées |
| Lecture headers A/B | Résultats de lecture et validation de chaque copie |
| Autorité volume | Absente, sélectionnée, dégradée ou ambiguë |
| Association TR2 / volume | Identité locale, propriétaire, identité volume et génération acceptée |
| Géométrie | Alignements, bornes et capacité nécessaire |
| Backend bulk | Initialisation, cohérence globale, activité et recovery requis |
| Résultats recovery | Résultats globaux/individuels et préfixes durables |
| Ressources disponibles | Slots et capacité d'allocation objectivement établis |
| Validité temporelle | NOT_EVALUATED, CURRENT ou INVALIDATED |
| Diagnostics | Causes cumulées et cause principale dérivée |
| Décisions | Accès diagnostique, usage opérationnel et nouvelle campagne |

UNKNOWN n'est jamais favorable par défaut. UNKNOWN, UNAVAILABLE et FOREIGN
ne sont pas CORRUPTED. Une corruption doit reposer sur une incohérence ou
une altération effectivement détectée, avec sa portée conservée.

## 4. Validité temporelle et décisions distinctes

Trois états conceptuels sont gelés :

- NOT_EVALUATED : évaluation nécessaire non effectuée ;
- CURRENT : évaluation valable pour la session média concernée ;
- INVALIDATED : évaluation rendue caduque, réévaluation requise.

NOT_EVALUATED et INVALIDATED interdisent l'utilisation opérationnelle.
CURRENT ne signifie favorable que si les autres dimensions nécessaires sont
validées. La readiness est liée à la session média évaluée. Ne pas revendiquer
une détection instantanée du retrait sans preuve matérielle correspondante.

Trois décisions restent séparées :

1. accès diagnostique non destructif autorisable, selon les accès réellement
   disponibles et les bornes sûres ;
2. utilisation opérationnelle du stockage campagne ;
3. nouvelle campagne autorisable côté stockage.

L'accès diagnostique peut rester permis lorsque l'utilisation est refusée,
notamment pour identifier un volume étranger. Il n'autorise ni adoption ni
accès à une fenêtre dont les bornes ne sont pas établies. Un média absent ou
inaccessible reste diagnostiquable par les faits déjà disponibles, même si
aucune nouvelle lecture SD ne peut aboutir.

La communication TR2 n'est pas rendue indisponible par la seule indisponibilité
bulk ; ses propres dépendances doivent néanmoins fonctionner. START global
conserve les autres critères applicatifs et transactionnels. Aucun quota
storage_limit_mb n'entre dans ces décisions D2.

## 5. Autorité volume et association : D1 inchangé

Vérifier l'identité logique TR2 valide, volume_identity,
owner_tr2_identity, génération acceptée côté FRAM, géométrie et règles A/B.
Toute divergence nécessaire interdit l'utilisation opérationnelle.

| Cas A/B D1 | Règle conservée |
|---|---|
| Deux copies valides, même génération, contenu identique | Autorité unique acceptable, sous contrôle de l'association |
| Même génération, contenu différent | Ambiguïté : refus |
| Générations différentes | Supérieure sélectionnable seulement si invariants immuables cohérents |
| Identité volume ou géométrie incompatibles entre copies valides | Refus, sans sélection aveugle |
| Une valide, autre structurellement invalide/corrompue | Dégradé potentiellement acceptable seulement sous les trois égalités exactes ci-dessous |
| Une copie impossible à lire | Refus d'utilisation |
| Une version non supportée | Refus, sans repli sur une ancienne version |
| Deux copies vierges | Non provisionné, sans provisioning automatique |
| Aucune valide | Refus, sans réparation au boot |

Les invariants de géométrie entre copies sont volume_length_bytes,
bulk_offset_bytes et bulk_length_bytes. En dégradé, exiger simultanément
volume_identity égale à l'association persistée, owner_tr2_identity égale
au TR2 et generation égale à la génération acceptée côté TR2.
Toute autorité sélectionnée reste soumise à l'association ; ne jamais choisir
une ancienne copie parce que la plus récente ne correspond pas à la FRAM.
Les validations de version, record_length, reserved et CRC restent celles D1.

Un propriétaire différent ne démontre FOREIGN que si l'identité locale et
l'autorité volume sont valides. Sinon, conserver l'indétermination.
Distinguer association absente, volume attendu différent, génération plus
ancienne et génération plus récente que celle acceptée. Aucun de ces cas
n'autorise une adoption, mise à jour FRAM ou réassociation automatique.

## 6. Recovery global et sémantique des contenus

Une initialisation backend réussie ne prouve pas sa readiness. Avant readiness
opérationnelle, exiger une vérification globale du layout bulk : descriptors,
conflits d'identifiants, recouvrements, cohérence des extents nécessaires,
résultats de recovery et ressources nécessaires. Ne pas la limiter aux
campagnes connues du repository FRAM.

La future frontière d'observation doit rendre cette vérification possible
sans exposer les structures internes D4-C à SystemRuntime. L'API exacte reste
à concevoir ; aucune signature ni extension de format n'est imposée ici.
Un indicateur interne recovery_required seul n'est pas une synthèse de santé.

| Notion | Sens distinct |
|---|---|
| Volume non provisionné | Deux headers reconnus vierges, sans autorité volume |
| Contenu non reconnu | Contenu lisible sans autorité TR2 reconnue ; pas preuve de virginité |
| Slot bulk EMPTY | Aucun descriptor campagne dans ce slot selon D4-C |
| Campagne absente pour un ID | Recovery de cet ID sans entrée trouvée |
| Inventaire bulk global vide | Vérification complète établissant absence de campagnes bulk |
| Repository FRAM vide | Absence de campagnes dans cette autorité, sans conclusion sur la SD |
| OPEN historique | État persistant d'une campagne interrompue, distinct de l'activité courante |
| Campagne actuellement active | État runtime/backend courant |
| Prêt pour nouvelle campagne | Décision dérivée, pas synonyme d'un contenu EMPTY |

VALID avec préfixe nul n'est pas EMPTY. Une campagne OPEN récupérée avec
préfixe durable valide n'est pas automatiquement corrompue et n'est pas
reprise automatiquement. OPEN historique ne signifie pas campagne active.
Un OPEN cohérent ne bloque pas nécessairement une nouvelle campagne si les
autres conditions sont satisfaites. FINISHED ne libère implicitement ni slot
ni extents. UNAVAILABLE reste une impossibilité d'établir la preuve ;
CORRUPTED et UNSUPPORTED restent des résultats distincts.

## 7. Divergences FRAM / bulk et nouvelle campagne

Une incohérence inexpliquée entre les autorités interdit toute nouvelle
campagne jusqu'à qualification de la situation. Ne pas déclarer par ce seul
fait toutes les données historiques illisibles. Conserver les résultats et
les garanties établis pour chaque donnée et accès.

Distinguer absence explicable par une fenêtre transactionnelle, incohérence
avérée et situation indéterminée. OPEN FRAM avec entrée bulk absente peut
résulter d'une coupure entre open_campaign() et begin_campaign() ; ne pas le
classer automatiquement CORRUPTED. La seule plausibilité de cette fenêtre
ne qualifie pas automatiquement l'absence. Une situation indéterminée
n'autorise pas START. Les règles détaillées de réconciliation restent à
concevoir ; aucune fermeture, migration ou correction persistante implicite.

Conditions nécessaires pour une nouvelle campagne, côté stockage :

- évaluation CURRENT et favorable ;
- autorité volume et association valides ;
- backend globalement cohérent ;
- aucune campagne active incompatible ;
- slot disponible établi ;
- capacité démontrée pour l'allocation minimale réellement requise par le
  backend et son profil technique.

Ne pas utiliser les fixtures E1/E3/E4 pour dimensionner la production.
La borne structurelle D1 ne garantit pas à elle seule toute allocation selon
le profil choisi. Ne pas garantir l'espace nécessaire à toute la campagne.
Quota, réservation et admission selon storage_limit_mb restent E.

## 8. Invalidation dynamique

Les événements suivants invalident l'évaluation concernée : retrait détecté,
erreur I/O pertinente, timeout, échec append/checkpoint/finish,
réinitialisation périphérique, changement de capacité ou d'identité détecté,
et perte de cohérence nécessaire.

Conserver la cause, l'opération et les faits observés. Une erreur logicielle
d'argument n'est pas automatiquement une panne média ou une corruption.
L'échec ne permet pas de poursuivre opérationnellement sous une ancienne
évaluation favorable. Réévaluation explicite et contrôlée ; aucun retry,
repair ou fallback automatique n'est créé par D2. Au reboot, ne pas hériter
implicitement d'une ancienne readiness favorable : refaire les vérifications
nécessaires non destructives.

## 9. Frontières de responsabilité et B1/B3

| Composant futur | Responsabilité gelée |
|---|---|
| Adaptateur média | Observations physiques et erreurs |
| Couche volume | Validation D1, headers A/B et géométrie |
| Évaluateur readiness | Association, cohérence, validité et décisions |
| CampaignDataStoreBulk | Layout campagne, recovery et ressources |
| SystemRuntime / CampaignService | Consommer les décisions, garde START, propagation des erreurs et invalidation |
| Supervision / diagnostic | Projection des diagnostics réellement établis |

Ces frontières ne figent pas de signatures API ni une chaîne d'appels linéaire.
Conserver l'injection obligatoire et l'ownership externe du gel A. La garde
START doit couvrir les chemins concernés avant les effets campagne évitables,
sans redéfinir la politique globale ou le protocole transactionnel des commandes.

Gap actuel : B1 annonce une disponibilité stockage inconditionnelle.
La future disponibilité doit dériver d'observations réelles ; ce gel ne
corrige pas le producteur actuel. B3 n'est pas une preuve de readiness stockage.
Un résultat de supervision ancien ne prouve pas l'accessibilité actuelle SD.
Aucun registre nouveau ni changement du mapping V1 gelé n'est introduit.

ConfigurationValidationEnvironment reste une caractérisation de capacité
pour validation configuration, distincte de l'association, de la readiness
dynamique et de la réservation de quota. D2 peut vérifier géométrie, capacité
structurelle et espace objectivement disponible si fiable, sans décider E.

## 10. Matrice des décisions

Les diagnostics sont conceptuels, cumulables et sans valeurs numériques.
« Conditionnel » signifie que toutes les autres dimensions nécessaires sont
favorables dans le même snapshot ; UNKNOWN ne satisfait aucune condition.
« Diagnostic oui » autorise seulement les observations sûres possibles et
la consultation des faits conservés, sans promettre une lecture SD réussie.
Dans tous les cas, la communication TR2 est conservée si ses dépendances
propres le permettent ; un refus stockage seul ne doit pas la supprimer.

| Cas | Observations nécessaires | Diagnostic | Usage opérationnel | Nouvelle campagne | Diagnostic non destructif | Action automatique éventuelle |
|---|---|---|---|---|---|---|
| Absent | Absence démontrée | MEDIA_ABSENT | Non | Non | Oui, faits disponibles | Publication ; pas d'accès SD promis |
| Inaccessible | Init/lecture impossible, erreur conservée | UNAVAILABLE | Non | Non | Oui, selon accès possible | Publication ; pas de retry |
| Non provisionné | Deux headers reconnus vierges | UNPROVISIONED | Non | Non | Oui | Pas de provisioning |
| Contenu inconnu | Aucune autorité reconnue, magic inconnue | UNRECOGNIZED_CONTENT | Non | Non | Oui | Refus sans formatage |
| Version incompatible | Version non supportée sur une copie | INCOMPATIBLE | Non | Non | Oui | Pas de repli/migration |
| Header corrompu | CRC/structure invalides, aucune autorité acceptable | HEADER_INVALID | Non | Non | Oui | Pas de réparation |
| Autorité A/B ambiguë | Conflit valide de génération/contenu/identité/géométrie | AUTHORITY_CONFLICT | Non | Non | Oui | Refus |
| Redondance dégradée acceptable | Une valide, autre invalide ; trois égalités D1 exactes | REDUNDANCY_DEGRADED | Conditionnel | Conditionnel | Oui | Poursuivre évaluation, pas réécrire |
| Une copie illisible | Erreur de lecture A ou B | HEADER_IO_UNAVAILABLE | Non | Non | Oui, faits accessibles | Refus même si autre valide |
| Volume étranger | Autorité valide, propriétaire différent du TR2 connu | FOREIGN | Non | Non | Oui | Identifier, pas adopter |
| Association manquante | Autorité valide, association FRAM absente | ASSOCIATION_MISSING | Non | Non | Oui | Pas d'association automatique |
| Identité locale absente/invalide | Identité TR2 non validée | LOCAL_IDENTITY_UNAVAILABLE/INVALID | Non | Non | Oui | Relation indéterminée |
| Volume associé différent | Owner correct, volume_identity différente | ASSOCIATION_MISMATCH | Non | Non | Oui | Pas d'adoption |
| Génération divergente | Comparaison exacte FRAM/header | GENERATION_BEHIND ou AHEAD | Non | Non | Oui | Pas d'acquittement automatique |
| Géométrie invalide | Bornes/alignements/capacité D1 non satisfaits | GEOMETRY_INVALID / REGION_TOO_SMALL | Non | Non | Oui, sans fenêtre non bornée | Refus |
| Recovery requis | Vérification nécessaire non achevée | RECOVERY_REQUIRED | Non | Non | Oui | Recovery non destructif prévu |
| Recovery réussi | Global cohérent, résultats nécessaires établis | BACKEND_RECOVERED | Conditionnel | Conditionnel | Oui | Calcul décisions/ressources |
| OPEN historique | Descriptor/préfixe cohérents, pas activité actuelle | INTERRUPTED_CAMPAIGN | Conditionnel | Conditionnel | Oui | Conserver, pas reprendre |
| Divergence FRAM/bulk | Autorités comparées, explication non qualifiée | INCONSISTENCY ou UNDETERMINED | Selon garanties établies ; pas d'usage ambigu | Non | Oui | Diagnostiquer, pas réconcilier implicitement |
| Absence de slot | Inventaire fiable sans slot libre | NO_FREE_SLOT | Possible pour opérations restantes valides | Non | Oui | Aucun reclaim |
| Espace insuffisant | Allocation minimale non réalisable | NO_APPEND_SPACE | Selon opérations et capacité nécessaires | Non | Oui | Aucun nettoyage |
| Erreur I/O runtime | Échec opération observé | RUNTIME_IO_FAILURE | Non jusqu'à réévaluation | Non | Oui | Invalider, propager |
| Readiness invalidée | Événement invalidant conservé | INVALIDATED | Non | Non | Oui | Réévaluation contrôlée seulement |
| Non évalué | Observations nécessaires manquantes | NOT_EVALUATED / UNKNOWN | Non | Non | Oui | Évaluation non destructive prévue |
| Stockage opérationnel | CURRENT, toutes preuves nécessaires favorables | OPERATIONAL | Oui pour opérations compatibles | Seulement si conditions section 7 | Oui | Opérations normales autorisées par contexte |

La sélection d'autorité ou un recovery réussi ne suffit pas isolément.
Un stockage sans campagne peut devenir opérationnel après vérification
complète ; EMPTY seul n'autorise jamais START.

## 11. Actions automatiques et frontières différées

Autorisables au boot : lecture, validation, comparaison, recovery non
destructif prévu, calcul readiness et publication diagnostics.

Interdites sans opération explicite : provisioning, adoption, réassociation,
formatage, réparation, migration, reclaim, nettoyage, reprise automatique OPEN
et réécriture corrective d'un header A/B. Dégradé n'autorise pas auto-réparation.
Aucun fallback historique ; aucune écriture d'essai implicite pour fabriquer
une preuve de disponibilité.

D3 définit les opérations persistantes explicites : première mise en service,
carte de remplacement, volume étranger, perte d'association, remplacement
CPU/FRAM, reprovisionnement et éventuelle récupération administrative.
D2 ne conçoit ni création des identités, association/réassociation, protocole
FRAM/microSD, migration ni résolution opérationnelle détaillée des divergences.
D4 porte l'intégration production et les signatures nécessaires après conception.
E conserve quota, réservation et admission storage_limit_mb. La politique
globale START reste séparée. Aucune stratégie acquisition/buffering/stalls.

## 12. Compatibilité et validation documentaire

D2 ne modifie ni format campagne D4-C, ni format volume D1, ni format/offset
FRAM, ni firmware, test, CMake, SystemRuntime ou mapping Modbus. La réservation
historique reste conservée. E4 est inchangé, avec baseline physique exacte
`111ca804c08cd126abc0ae10ff2a923ce7554ebe` ; aucune qualification physique
n'est annoncée par ce gel. Le développement continue sur main.

La validation porte sur la correspondance au contrat D2-F, la cohérence
A/D0/D1, le maintien des règles A/B, la séparation observation/diagnostic/
validité/décision, les frontières D2/D3/E, l'inspection du diff complet et
`git diff --check`. Une compilation ou des tests firmware n'apporteraient
aucune preuve supplémentaire à cette modification documentaire ; aucun
résultat d'implémentation ou matériel n'est revendiqué.
