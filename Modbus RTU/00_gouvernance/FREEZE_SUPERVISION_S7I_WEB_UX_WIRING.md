# Gel S7-I — Câblage final de l’UX Web réelle

## Statut

S7-I est clôturée après validation locale des tranches I1, I2 et I3.

## Baseline gelée

Dernier état fonctionnel validé avant ce gel :

`5d798180df98e0563b60a18543d86a1218ecb52f`

`Supervision: add S7-I3 UX coherence`

## Objet

S7-I aligne l’IHM Web réelle produite en S7 avec l’expérience opérateur gelée en S6-H, sans modifier la spécification Modbus V1, le firmware ni les autorités runtime.

## Résultat

Navigation globale réelle :

```text
Vue générale
Vibrations
Attention & diagnostic
Campagnes
Système
```

Navigation contextuelle TR2 réelle :

```text
Synthèse
Vibrations
État & configuration
Commandes
Campagnes
Diagnostic
```

Les pages globales utilisent les projections/API existantes. Aucun accès Modbus direct n’est introduit dans le navigateur.

## Conformité des actions

- les commandes B5 passent exclusivement par le moteur transactionnel existant ;
- `Prepared`, `Submitted` et `Ambiguous` bloquent les nouvelles commandes ;
- `TerminalEvidenceObserved` ne signifie pas automatiquement succès métier ;
- aucun Retry / Ignore / Force n’est exposé ;
- aucun acquittement global n’est exposé ;
- seul l’acquittement ciblé par code défaut reste visible ;
- `RESET_STATISTICS` reste absent ;
- la clé de confirmation B5 reste interne à la supervision ;
- B6 reste distinct de B5.

## Cohérence des données

- dernière valeur connue conservée et qualifiée après indisponibilité ;
- fraîcheur PC distincte de la validité/fraîcheur TR2 ;
- B1, B3, B7, communication PC et état transactionnel B5 restent des sources distinctes ;
- aucune sévérité universelle n’est créée ;
- aucune vitesse mm/s, FFT, fréquence dominante, seuil ISO ou score santé n’est inventé ;
- RMS et crête B3 restent les valeurs fournies par le TR2 ;
- `duration_s` B6 reste l’autorité de durée affichée.

## Gestion d’indisponibilité Web

Les erreurs de lecture de l’historique B5 sur la vue globale `Attention & diagnostic` sont désormais visibles comme indisponibilité de supervision. Elles ne sont ni masquées comme « aucune attention », ni transformées en défaut TR2, ni requalifiées artificiellement en `Ambiguous`.

## Frontières inchangées

S7-I ne crée :

- aucun second runtime ;
- aucun cache métier parallèle ;
- aucun nouvel endpoint métier ;
- aucune nouvelle règle V1 ;
- aucune nouvelle FW_POLICY ;
- aucune conclusion matérielle pré-hardware.

## Validation

Les tranches I1, I2 et I3 ont été compilées et testées localement par l’utilisateur. Le dernier retour valide l’ensemble des tests existants.

## Suite

S7-J est consacré à l’intégration multi-client et à la concurrence Web. Les structures historiquement mono-flux doivent y être auditées explicitement avant de considérer plusieurs requêtes HTTP simultanées comme sûres.

Le premier point identifié pour S7-J est la présence de collections mutables non synchronisées dans le chemin opérationnel (`CommandCoordinatorRegistry`, `SupervisionOperationalFacade`, `BusWorkScheduler`). Ce point n’appartient pas au périmètre UX de S7-I et doit être traité séparément.
