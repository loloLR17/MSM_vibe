# TR2 Supervision

Socle logiciel de la supervision multi-capteurs TR2.

## S1-A

Cette tranche établit uniquement les frontières de solution .NET 10 :

- `TR2.Domain` : concepts métier indépendants des infrastructures ;
- `TR2.Transport` : frontière de transport, sans connaissance B0-B7 ;
- `TR2.Protocol` : protocole TR2, dépend de Domain et Transport ;
- `TR2.Application` : orchestration de supervision ;
- `TR2.Persistence` : adaptateurs de persistance locale ;
- `TR2.Campaigns` : frontière d'import/archivage campagne ;
- `TR2.Supervision.Service` : host central et composition root.

S1-A n'implémente ni port série réel, ni Modbus RTU physique, ni mapping complet
B0-B7, ni base de données, ni parser SD, ni IHM Web.

## Validation

```bash
dotnet restore TR2.Supervision.sln
dotnet build TR2.Supervision.sln --no-restore
dotnet test TR2.Supervision.sln --no-build
```

## Diagnostic de banc M014-A

Le service accepte `--diagnostic` : OFFLINE par défaut, sans ouverture série ni
runtime de production. `--read-once` est réservé à une future mission autorisant
une lecture FC03/1000/1, avec paramètres explicites et journal exclusif.
UNKNOWN(0) ne constitue ni READY ni identification production.
Voir [audit, garanties et procédure HP préparée](../00_gouvernance/DIAGNOSTIC_SUPERVISION_M014_A.md).
