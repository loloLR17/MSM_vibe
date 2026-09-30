# P12-H3h-D3 — Barrière de checkpoint durable bulk

## Statut

Validation logicielle locale : **VERTE**.

Baseline validée :

```text
5723aa86ad72d65e2c1b042a2a3a9688cfdd2055
Firmware: wire H3h-D3 checkpoint tests
```

## Objet

Matérialiser dans le core la distinction entre données écrites au média et
préfixe autorisé à être déclaré durable au niveau de la session courante.

## Séquence

`campaign_bulk_checkpoint_publish()` applique strictement :

1. flush de la queue RAM de `CampaignBulkAggregator` ;
2. calcul du préfixe candidat effectivement écrit ;
3. `CampaignBulkMedia::sync()` ;
4. publication en RAM de `durable_prefix_bytes` uniquement après succès du
   sync.

Une écriture média, y compris une écriture automatique d'un buffer plein, ne
constitue donc jamais à elle seule un checkpoint durable.

## Validation

Les tests unitaires couvrent :

- flush de la queue avant sync ;
- publication après sync réussi ;
- buffer plein déjà écrit mais non durable avant checkpoint ;
- échec de flush : aucun sync et aucun avancement du préfixe durable ;
- échec de sync après écriture : le préfixe écrit peut avancer tandis que le
  préfixe durable reste exactement au checkpoint précédent ;
- verrouillage de la couche checkpoint après erreur.

La validation complète `tr2_validate.sh` a été exécutée localement et déclarée
entièrement verte.

## Limite volontaire

Le `durable_prefix_bytes` de H3h-D3 est un état RAM de la session courante.
H3h-D3 ne fournit encore aucune autorité de recovery après reset ou power-loss.

En particulier, le succès du sync des données ne suffit pas à rendre le nombre
`durable_prefix_bytes` lui-même récupérable : une métadonnée persistante doit
encore publier cette autorité.

## Suite

H3h-D4 doit définir puis tester la publication persistante et recoverable des
métadonnées de campagne. Son ordre transactionnel devra préserver le dernier
checkpoint prouvé en cas de coupure pendant l'écriture ou la publication des
métadonnées.
