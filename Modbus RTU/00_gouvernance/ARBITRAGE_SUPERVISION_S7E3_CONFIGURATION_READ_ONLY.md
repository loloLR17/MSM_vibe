# Arbitrage supervision S7-E3 — Configuration B4 read-only

## Statut

Tranche S7-E3. Cette tranche reste en lecture seule et ne modifie aucun comportement Modbus V1.

## Constat

Le polling `Slow` lit déjà le bloc B4 via `PollingExecutor`, mais `PhysicalPollingWorkRunner` ne publie pas encore le résultat dans l'autorité de snapshots utilisée par la supervision Web.

## Décision

1. Étendre `DeviceTelemetrySnapshots` avec un `ObservedSnapshot<B4ConfigurationState>`.
2. Publier `PollingReadSet.B4` dans `DeviceTelemetrySnapshotRegistry` pour les sessions compatibles.
3. Appliquer les mêmes règles de disponibilité et de fraîcheur PC que B1/B2/B3/B7.
4. Exposer B4 uniquement à travers `IhmConfigurationStateReadModel` dans la projection existante.
5. Afficher la configuration dans la page détail TR2 existante ; le navigateur ne déclenche aucune lecture Modbus.

## Invariants

- une seule composition runtime et une seule autorité Modbus ;
- aucune lecture Modbus directe depuis le Web ;
- aucune écriture B4/B5 ajoutée ;
- aucune règle V1.1 inventée ;
- les valeurs `Prepared*` et `Active*` restent distinctes ;
- les codes et masques sont exposés tels que lus, sans interprétation métier non normative ;
- une perte de communication conserve la dernière configuration connue mais la marque indisponible ;
- B6 reste hors de cette tranche et sera traité en S7-F.

## Périmètre IHM

La page détail ajoute une vue `Configuration` présentant les métadonnées de configuration, les paramètres préparés et actifs, les seuils B4 en mg, ainsi que les contextes/labels. Il n'existe aucun bouton d'application ou de modification dans S7-E3.

## Validation attendue

```bash
git pull --ff-only origin main
dotnet build TR2.Supervision.sln --no-restore
dotnet test TR2.Supervision.sln --no-build
```
