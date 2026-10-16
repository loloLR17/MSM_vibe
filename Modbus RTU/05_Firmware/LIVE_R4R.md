# R4-R — temps événement physique, banc isolé

Mission TR2-20261016-R4R-PHYSICAL-TIME-B3, initial9dc28db. Première tranche :
probe explicite NON-PRODUCTION, capteur/temps seuls pendant2s ; aucune entrée
UART/SD/commande R2, aucune publication B3 avant justification du temps.
Préflight complet Flash2Mio c5bc344d…f5b26 et FRAM262144 a13b3401…a2ef9,
UID/STLINK qualifiés. L'archive R4-Q est byte-identique à650aac8.

Le probe réutilise propriétaire/parsing/gardes SPI qualifiés ; contrôle statuts,
saturation, tags et intervalles TS/8 inchangés. Le numéro diagnostic volatile
UINT64_MAX-1 n'est ni un run R2 réservé ni une autorité produit ; aucune identité
persistante n'est consommée. Le premier mot a une allowance expérimentale20ms,
distincte du gap5ms. Chaque service est mesuré et arrêté au-delà2500us.

Collecte prévue : premier préfixe2048mots bruts, tous les deltas timestamps,
compteurs d'association TAG_CNT et champs réservés, compteur interne lu avec
bracketing DWT toutes10ms. Le probe conserve les états et s'arrête ; aucune
validation physique n'est encore revendiquée dans cette première version.
L'analyse indépendante Python fournit ses propres oracles LE32/rollover/parité
et pente ; aucun nominal26667 ne sert à fabriquer un événement. Bibliothèque
ST et parseur V1 ne sont pas modifiés.

Références constructeur consultées : ST DS12569 Rev8 §9.28 (timestamp/fine),
AN5444 Rev3 §6.2/6.6 (slots, TAG_CNT et corrélation hybride). Une pente sensor→DWT
mesurée et une association observée restent nécessaires ; UTC non synchronisée.
Le raccordement B3, la charge FRAM et les budgets conjoints attendent ce résultat.
