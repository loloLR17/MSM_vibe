# Gel P12 / H3h — D0 — Volume microSD de production, readiness et provisioning

## 1. Statut, portée et références

**CONTRAT ARCHITECTURAL GELÉ — DOCUMENTAIRE UNIQUEMENT.**

Baseline examinée : `19f39e926d1e82a69163688f75ebc1dd9ff47c00`.
D0 fixe les décisions de volume, d'association et d'autorisation d'usage du
stockage. Il ne déclare pas ces mécanismes implémentés ou qualifiés.

Références normatives et preuves techniques :

- [Gel de composition production A](FREEZE_FIRMWARE_P12H3H_PRODUCTION_CAMPAIGN_STORAGE_COMPOSITION_A.md) ;
- [Arbitrage bulk H3h-A](ARBITRAGE_FIRMWARE_P12H3H_A_CAMPAIGN_BULK_STORAGE.md) ;
- [Layout campagne D4-C](CONCEPTION_FIRMWARE_P12H3H_D4C_MULTI_CAMPAIGN_BULK_LAYOUT.md) ;
- [Contrat de données P8-B](FREEZE_FIRMWARE_P8B_CAMPAIGN_DATA_CONTRACT.md) ;
- [Validation E1](VALIDATION_FIRMWARE_P12H3H_E1_SDMMC_BULK_MEDIA.md) ;
- [Validation E2](VALIDATION_FIRMWARE_P12H3H_E2_SECTOR_ISOLATION.md) ;
- [Validation physique E3](VALIDATION_FIRMWARE_P12H3H_E3_D5C_SDMMC_END_TO_END.md) ;
- [Procédure physique E4](PROCEDURE_FIRMWARE_P12H3H_E4_PHYSICAL_POWERLOSS_RECOVERY.md).

D0 complète les décisions différées à D dans le gel A. Il conserve l'injection
obligatoire du `CampaignDataStore`, l'ownership externe du backend, du média et
des buffers, et la même instance/interface pour recovery et `CampaignService`.
Les opérations restent synchrones, potentiellement bloquantes, sans borne de
latence revendiquée ni retry automatique.

La numérotation D0..D5 ci-dessous concerne la composition de production ; elle
ne renomme pas les sous-tranches historiques H3h-D1..D5 ou le format D4-C.

## 2. Carte dédiée et région explicitement autorisée

La microSD de production est dédiée au TR2.

`CampaignDataStoreBulk` ne reçoit jamais implicitement toute la capacité de
la carte physique. La composition lui fournit une région de production
explicitement autorisée et bornée. Les limites de cette région font partie du
contrat d'utilisation du volume.

Les fenêtres et harness E1/E3/E4 restent exclusivement des dispositifs de
qualification. Leurs offsets, tailles, marqueurs et valeurs de fixture D5-C
ne définissent aucune géométrie ou capacité de production.

La géométrie numérique exacte de production n'est pas figée par D0.

## 3. Identité persistante du volume TR2

Une carte de production doit être explicitement provisionnée comme volume TR2.
Le volume possède une identité persistante propre.

Cette identité ne peut être déduite :

- du `campaign_id` ;
- du contenu arbitraire de la carte ;
- de metadata D4-C classées EMPTY ;
- de la seule capacité physique du support.

D0 gèle le besoin d'identité. Le mécanisme persistant exact, sa structure
binaire, ses offsets, son CRC, sa version et son éventuelle redondance seront
conçus dans une tranche D ultérieure. Aucun encodage concret n'est choisi ici.

## 4. Association explicite au TR2 — choix V1

**Une carte de production est liée explicitement à un TR2.**

Le TR2 conserve côté persistance transactionnelle les informations nécessaires
pour reconnaître le volume auquel il est associé.

Une carte provisionnée pour un autre TR2 doit être reconnue comme un volume
TR2 valide mais étranger. Elle ne doit jamais être adoptée automatiquement.
Son utilisation nécessite une opération humaine explicite dont la procédure
sera définie ultérieurement.

Aucune association automatique n'est effectuée au boot. Le partage d'un
`campaign_id` entre les deux domaines ne prouve pas l'association au volume.

## 5. Provisioning distinct du boot

Une carte vierge ou inconnue n'est pas utilisable automatiquement.

Le provisioning est explicite, volontaire, potentiellement destructif et
séparé du boot normal. Sa procédure exacte et son interface seront conçues
ultérieurement avant implémentation.

Le boot normal ne doit jamais automatiquement :

- formater ;
- nettoyer ;
- réparer ;
- adopter ;
- reprovisionner une carte.

La reconnaissance d'un contenu EMPTY ne constitue ni une autorisation de
provisioning ni une preuve que la carte appartient au TR2.

Les scénarios de changement, remplacement et provisionnement de carte devront
être explicitement conçus, y compris leurs conséquences sur les deux domaines
persistants. D0 ne définit aucune procédure implicite de migration ou de remise
en état.

## 6. Trois notions de readiness et d'admission

| Notion | Contrat gelé |
|---|---|
| System / communication readiness | Le TR2 peut rester opérationnel et diagnostiquable par Modbus même si le stockage bulk n'est pas utilisable |
| Bulk storage readiness | Le média est reconnu et satisfait les conditions nécessaires à son utilisation comme volume TR2 associé |
| Campaign admission | Autorisation de commencer une nouvelle campagne, distincte de la readiness du système et du stockage |

Un stockage non ready interdit START d'une nouvelle campagne, sans imposer à
lui seul la perte de communication Modbus.

La readiness bulk est une condition nécessaire à START ; elle n'est pas à elle
seule une admission acquise. Les critères supplémentaires existants et les
futurs arbitrages de quota restent distincts.

Ni le succès d'une initialisation structurelle ni la seule readiness Modbus
ne doivent être présentés comme une preuve d'utilisabilité du stockage bulk.
Le modèle précis, ses transitions et son intégration seront définis après D0.

## 7. Refus par défaut — fail closed

Toute ambiguïté concernant la présence, l'identité, l'association, la
compatibilité, l'intégrité nécessaire ou la capacité nécessaire conduit au
refus d'utilisation du stockage concerné jusqu'à résolution.

Ce refus ne doit pas être transformé en succès, autorisation automatique ou
preuve de corruption lorsqu'il s'agit seulement d'une information insuffisante.
La situation doit rester diagnostiquable selon le futur modèle de readiness.

Aucun fallback automatique vers `CampaignDataStorePersistent` n'est autorisé.
Ce backend reste historique et explicitement composé conformément au gel A ;
il n'est ni cache ni substitut implicite du bulk.

## 8. Situations diagnostiques cibles

Les situations suivantes doivent pouvoir être distinguées conceptuellement :

| Situation | Signification attendue |
|---|---|
| Média absent | Aucun média disponible pour le stockage concerné |
| Média inaccessible / erreur I/O | L'accès au média ne permet pas les vérifications ou opérations nécessaires |
| Média présent mais non provisionné | Le média n'est pas reconnu comme volume TR2 explicitement provisionné |
| Volume TR2 provisionné et associé | Identité et association au TR2 reconnues ; les autres conditions de readiness restent à vérifier |
| Volume TR2 valide mais étranger | Volume reconnu, associé à un autre TR2 ; aucune adoption automatique |
| Volume / version incompatible | Le contrat nécessaire à l'utilisation n'est pas supporté |
| Metadata / volume corrompu | Une incohérence ou corruption nécessaire à l'usage a été effectivement détectée |
| Capacité / région insuffisante | La région autorisée ne satisfait pas les besoins nécessaires à l'usage |
| Recovery requis | L'état nécessite le recovery prévu avant toute utilisation autorisée |
| Stockage prêt pour nouvelle campagne | Les conditions de stockage nécessaires à une nouvelle campagne sont satisfaites ; l'admission globale reste distincte |

Cette table n'impose ni un enum unique, ni des états mutuellement exclusifs,
ni des valeurs numériques. Les règles de priorité, transitions et présentation
Modbus seront conçues ultérieurement. D0 ne crée aucun mapping ou commande
Modbus et ne revendique pas que ces distinctions soient déjà implémentées.

## 9. Capacité et domaines de géométrie

Les notions suivantes restent explicitement séparées :

| Notion | Sens |
|---|---|
| Capacité physique microSD | Taille effectivement détectée du support |
| Région physique / logique réservée au TR2 | Périmètre explicitement autorisé pour le volume de production |
| Overhead de volume | Espace nécessaire aux mécanismes de volume à concevoir |
| Région `CampaignDataStoreBulk` | Vue bornée fournie au backend des données campagne |
| Overhead campagne | Descriptors, headers, CRC et alignements du format existant |
| Espace durable déjà consommé | Extents persistants conservés, distincts du quota d'une nouvelle campagne |
| Espace encore disponible | Espace restant dans la région autorisée, après les réserves applicables |
| `storage_limit_mb` | Plafond logique configuré pour une campagne |

Aucune taille numérique de carte ou de région, aucun offset production et
aucune formule définitive de capacité publiable ne sont fixés ici.
La capacité physique seule ne constitue ni la capacité logique exploitable,
ni une preuve de provisioning, ni une garantie d'admission.
Les exigences de caractérisation honnête de `ConfigurationValidationEnvironment`
du gel A et de l'arbitrage H3h-A restent applicables.

## 10. Plafond campagne et frontière avec E

Le gel A est conservé : `storage_limit_mb` est un plafond logique par campagne
et un MB signifie **1 000 000 octets**.

La réservation, l'admission liée au quota et le comportement exact à l'atteinte
du plafond restent en tranche E. D0 ne choisit ni réservation intégrale au
START ni réaction automatique au dépassement ou à l'épuisement du quota.
La disponibilité d'une région n'est pas une réservation de quota acquise.

## 11. Persistance transactionnelle et microSD

FRAM / persistance transactionnelle et microSD persistent séparément et
restent deux autorités distinctes. Leur cohérence n'est pas une transaction
atomique implicite.

D0 autorise la conception future des informations minimales nécessaires à
l'association TR2 ↔ volume. Il ne modifie aucun format ni offset FRAM.
La réservation historique data et tous les offsets existants restent conservés.
Aucune migration FRAM vers microSD implicite n'est introduite.

L'encodage, l'emplacement et les scénarios d'interruption des opérations
portant l'association devront être explicitement conçus avant implémentation.

## 12. Compatibilité avec les données campagne D4-C

Le format `CampaignDataStoreBulk` D4-C existant reste la baseline des données
campagne :

- 8 slots ;
- descriptors A/B ;
- extents append-only ;
- CRC ;
- recovery par préfixe durable.

D0 ne modifie pas ce format. L'identité du volume est conceptuellement
extérieure au format des campagnes, sauf décision explicite ultérieure.

Une campagne OPEN après reboot n'est pas reprise automatiquement. Aucun
nettoyage, réparation, rétention ou reclaim implicite n'est ajouté. Les
frontières append/checkpoint/finish et le contrat P8-B restent inchangés.

## 13. Qualifications H3h et baseline E4

E1/E2/E3 restent des preuves techniques existantes dans leurs périmètres
respectifs : E1 et E3 documentent des observations physiques ; E2 documente
l'isolation sectorielle et sa validation logicielle. D0 n'étend pas leur portée
à une composition de production ou à une vraie coupure d'alimentation.

**E4 physique reste pending.** Sa baseline firmware reste exactement :

`111ca804c08cd126abc0ae10ff2a923ce7554ebe`.

Aucune décision ni modification documentaire D0 ne modifie ou n'invalide le
harness E4 ou sa baseline. Celle-ci sera reconstruite exactement pour sa
qualification physique. Le développement continue sur main ; l'applicabilité
de cette preuve au futur firmware de production sera évaluée séparément.

## 14. Séquencement recommandé après D0

| Tranche production | Travail à concevoir / réaliser |
|---|---|
| D1 | Volume de production : identité, version, intégrité, emplacement, géométrie et région bulk |
| D2 | Modèle de readiness et diagnostic, distinctions, transitions et refus d'utilisation |
| D3 | Provisioning explicite et association TR2 / volume, scénarios de changement ou remplacement |
| D4 | Composition STM32 production et intégration `SystemRuntime` |
| D5 | Validation logicielle puis qualification matérielle |
| E | Quota / admission liés à `storage_limit_mb` |

Le découpage pourra être raffiné explicitement. Il ne doit pas déplacer
silencieusement les décisions E dans D. Les futures implémentations doivent
préserver le contrat A/B/C et disposer des décisions nécessaires avant de
modifier les formats ou les opérations persistantes concernés.

## 15. Non-objectifs et décisions non gelées

D0 n'introduit :

- aucun UUID concret ni structure binaire ;
- aucun offset production, taille de carte ou taille de région ;
- aucune commande ou représentation Modbus ;
- aucune politique de quota ;
- aucun reclaim ou politique de rétention ;
- aucune stratégie FIFO/DMA, buffering ou stalls ;
- aucun formatage automatique.

Restent à concevoir : encodage/intégrité/redondance de l'identité, géométrie
numérique, informations d'association transactionnelles, procédure et interface
de provisioning, modèle détaillé de readiness et calcul de capacité exploitable.
Le présent gel n'en constitue pas une spécification binaire implicite.

## 16. Validation documentaire de D0

La validation porte sur la conformité aux décisions approuvées, la cohérence
avec le gel A, l'absence de valeurs de qualification promues en valeurs de
production, l'inspection du diff complet et `git diff --check`.

Aucun firmware, test, CMake ou format persistant n'est modifié. Une compilation
n'apporterait aucune preuve supplémentaire à cette tranche documentaire ;
aucune compilation ni qualification matérielle n'est requise pour D0.
