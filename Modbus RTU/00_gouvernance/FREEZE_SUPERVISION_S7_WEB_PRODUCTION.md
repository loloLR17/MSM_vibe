# Gel final S7 — Supervision Web de production

## 1. Statut

**S7 — Supervision Web de production est gelée**, sous réserve de la validation locale du présent commit documentaire.

La baseline fonctionnelle validée avant ce gel est :

- commit `ad63fd746b8125ae6bbed4465e522b250bf74373`
- message `Supervision: close S7-K1 global acknowledgement exposure`
- validation locale : build et tests verts, **466/466**.

## 2. Portée du gel

Le gel couvre l’ensemble des tranches :

```text
S7-A  Architecture Web + politique d'exposition HTTP
S7-B  Contrats/projections IHM read-only
S7-C  Host HTTP + API read-only minimale
S7-D  Vue générale réelle
S7-E  Détail TR2 / vibration / diagnostic / configuration
S7-F  Campagnes / système / communications
S7-G  Write path B5 via autorité existante
S7-H  Ambiguous / erreurs / reconnexion / lifecycle B5
S7-I  Câblage final de l’UX S6-H
S7-J  Intégration / concurrence multi-clients
S7-K  Audit transversal final
```

Les arbitrages et gels S0 à S6 restent applicables.

## 3. Architecture gelée

L’architecture de référence est :

```text
Navigateur
  -> HTTP same-origin
  -> TR2.Supervision.Web
  -> projections / sinks IHM
  -> PhysicalSupervisionRuntime unique
  -> Application / BusWorkScheduler
  -> transport Modbus RTU unique
  -> RS-485 / TR2
```

Le Web ne possède jamais directement :

- le bus RS-485 ;
- NModbus ;
- le mapping de registres ;
- la génération des `transaction_id` B5 ;
- la clé de confirmation protégée ;
- une seconde machine d’état métier.

## 4. Lecture IHM

Les pages réelles couvrent les parcours globaux :

```text
Vue générale
Vibrations
Attention & diagnostic
Campagnes
Système
```

et les parcours contextuels TR2 :

```text
Synthèse
Vibrations
État & configuration
Commandes
Campagnes
Diagnostic
```

Les lectures dérivent des autorités runtime existantes et ne créent pas de cache métier parallèle.

La dernière valeur connue est conservée après indisponibilité ; sa disponibilité et sa fraîcheur PC sont qualifiées séparément.

## 5. Commandes B5

Le chemin Web B5 est gelé avec les règles suivantes :

- un seul `CommandCoordinator` par `device_id` ;
- une seule transaction non terminale par device ;
- états IHM : `Prepared`, `Submitted`, `Ambiguous`, `TerminalEvidenceObserved` ;
- aucun replay automatique ;
- post-submit et réconciliation en lecture B5 uniquement ;
- `Ambiguous` bloque toute nouvelle commande B5 ;
- reconnexion/B0 seuls ne résolvent jamais une transaction ;
- recovery restart restaure le blocage sans inventer d’événement historique ;
- aucune action Retry / Ignore / Force ;
- `TerminalEvidenceObserved` ne signifie pas automatiquement succès métier ;
- `RESET_STATISTICS` reste absent ;
- l’acquittement exposé par S7 est uniquement ciblé par `faultCode` ; aucun acquittement global n’est exposé ;
- `SoftwareReset` nécessite une confirmation explicite côté IHM/API, tandis que la clé technique reste interne.

## 6. B6 campagnes

B6 reste strictement distinct de B5 :

- sélection de campagne non transactionnelle B5 ;
- aucun `transaction_id` B5 ;
- aucun journal B5 artificiel ;
- aucun replay automatique ;
- l’inventaire affiché provient de B6 ;
- aucune extraction brute, FFT ou reconstruction de données campagne n’est ajoutée en S7.

## 7. Concurrence multi-client

S7-J fige :

- scheduler de bus protégé contre les accès concurrents ;
- un seul work actif par bus ;
- work IDs et séquences protégés ;
- contextes publiés avant consommation possible du work ;
- création de coordinator atomique ;
- préparation B5 sérialisée par `device_id` ;
- soumissions Web B5/B6 sérialisées par device, non globalement ;
- anti-duplication de reconciliation ;
- `requestIdentity` dupliqué sur un même device reporté comme conflit.

## 8. Politique réseau gelée

Configuration initiale :

- HTTP ;
- Web désactivé par défaut ;
- écoute par défaut `127.0.0.1:5080` ;
- non-loopback interdit sans `allowRemote=true` explicite ;
- same-origin ;
- pas de CORS par défaut ;
- pas d’authentification, ACL, rôles ou TLS introduits en S7.

Une exposition LAN sans authentification signifie que les clients atteignant le host peuvent solliciter les actions autorisées. Elle reste donc volontaire et explicite.

## 9. Frontière normative

S7 ne modifie pas :

- la spécification Modbus RTU V1 ;
- les blocs B0 à B7 ;
- le firmware ;
- les règles transactionnelles firmware ;
- le scope EMPTY de `RESET_STATISTICS` ;
- les décisions V1.1.

Aucune lacune V1 n’est comblée silencieusement avec une règle V1.1.

## 10. Analyse vibratoire

S7 n’invente aucun indicateur absent de V1 :

- pas de vitesse mm/s calculée ;
- pas de FFT/spectre ;
- pas de fréquence dominante ;
- pas de seuil ISO ;
- pas de score santé synthétique.

Les RMS/crêtes B3 restent les valeurs transmises par le TR2 et non des valeurs recalculées par le Web.

## 11. Validation et limites

La fermeture fonctionnelle précédant ce gel a été validée localement avec **466/466 tests verts**.

Cette validation reste pré-matériel. Elle ne qualifie pas :

- le STM32 cible ;
- le câblage et le transceiver RS-485 réels ;
- les timings réels terrain ;
- les conditions EMI/EMC ;
- la charge Web de grande échelle ;
- un déploiement Windows Service/installer ;
- une politique cybersécurité définitive.

## 12. Décision

Sous réserve que le présent commit documentaire compile et conserve les **466/466 tests verts**, la phase **S7 est considérée fermée et gelée**.

La suite ne doit pas rouvrir implicitement S7. Toute extension fonctionnelle, cybersécurité, packaging/déploiement ou validation matérielle doit être traitée comme une phase séparée avec son propre cadrage.
