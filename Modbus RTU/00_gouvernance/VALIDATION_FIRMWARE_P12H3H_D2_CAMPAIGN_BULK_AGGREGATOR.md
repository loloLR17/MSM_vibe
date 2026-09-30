# P12-H3h-D2 — Agrégation RAM des écritures bulk

## Statut

Validation logicielle locale : **VERTE**.

Baseline validée :

```text
cf98a388b0eebc69f7eb68c0399f6399bcc6557f
Firmware: wire H3h-D2 bulk aggregator tests
```

## Objet

Introduire une brique d'agrégation RAM indépendante du layout de campagne et
du HAL STM32, afin de transformer un flux de petits append en écritures bulk
plus grandes sur `CampaignBulkMedia`.

## Contrat

`CampaignBulkAggregator` :

- reçoit un buffer fourni par son appelant ;
- n'impose aucune capacité RAM particulière ;
- accumule les données jusqu'à remplissage du buffer ;
- écrit automatiquement un buffer plein ;
- permet `flush()` de la queue partielle ;
- avance son offset uniquement après une écriture média réussie ;
- passe en état faulted après une erreur d'écriture ;
- n'appelle jamais `CampaignBulkMedia::sync()`.

La taille du buffer d'agrégation n'est donc pas confondue avec la réserve RAM
qui sera ultérieurement nécessaire pour absorber les stalls du média physique.

## Validation

Les tests unitaires couvrent :

- fusion de petits append en écritures pleines ;
- conservation d'une queue partielle en RAM ;
- flush exact de cette queue ;
- absence de sync implicite ;
- progression correcte des offsets ;
- erreur média sans progression silencieuse de l'offset ;
- verrouillage de l'agrégateur après erreur ;
- rejet d'une étendue initiale incompatible avec la capacité média.

La validation complète `tr2_validate.sh` a été exécutée localement et déclarée
entièrement verte.

## Invariants

- aucun changement du record P8-B de 16 octets ;
- aucun changement de la frontière logique de checkpoint P8-B ;
- aucun layout microSD imposé ;
- aucun HAL STM32/SDMMC dans le core ;
- aucune taille de buffer de production gelée ;
- aucune déclaration de durabilité sur simple `write()` ou `flush()`.

## Suite

H3h-D3 doit introduire la frontière de checkpoint durable au-dessus de D1/D2 :
la queue doit d'abord être flushée, puis le média synchronisé, et le préfixe
durable ne peut être publié qu'après succès de cette séquence.
