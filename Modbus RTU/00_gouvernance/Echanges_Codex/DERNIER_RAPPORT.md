---
mission_id: TR2-20261013-M015-T-RESUME-PUBLICATION
status: BLOCKED
base_ref: main
base_sha: 7d762176b9fdc14d3c0cdce32bd6b3ce9d135d35
initial_head: 7f7fc2a4e41b9734fce65123a371fcef8f154639
result_sha: 7d762176b9fdc14d3c0cdce32bd6b3ce9d135d35
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Reprise T — acquis4MHz audité et préservé avant extension

Les bruts locaux sont retrouvés et vérifiés indépendamment :4096XYZ+512timestamps,4082tripletsdistincts,312traces/624snapshots,parité/counters/saturation/transport/FIFOflags zéro, reconstruction transaction↔RAM byte-identique. SHAraw04e071f2f851ddb48293da0d329b7bf96f995d689c87ff913409daf7be1a33c6 ;BIN0091c1e84bf7a351ec3f5d7ca4f00109ce72924633ff11a57ad0027750f0b2f3,ELF9a9d568b1032e650ede0de5c7558dfdbf0bd4c0522f9cd3817d49deb547d6b57. Flash relue exacte. T CPU160/SPI4 est VALIDÉ PHYSIQUE BORNÉ ;pas perte zéro globale/industrialisation.

RCC/PWR/Flash :HSIready,CCIPR3=10h(SPIHSI16),PCLK3div1,PLLCPU160,VOSR7C000/VOS1ready,Flashlatency4,CFG1div4. Cleanup0/XLoff/bypass/TSoff,faults0/detach. PremierpréflightGDB7 pour mauvaiseadresseSPI3 conservé ;corrigé46002000 avantuniquetrigger,aucunreset/flashsupplémentaire. SourceSPI MSI→HSI etAPB/latencechangés :isolementCPUimparfait, fréquencephysiqueSCKnonmesurée.

Marge recalculée :payload1,807226ms ;service actif jusqu'au premier statut suivant≈2,048578ms(max2,049513),budgetnominal4,266667ms. Cadence incluantattentes4,285707ms ;elle suitl'arrivéeFIFO etn'estpasleplafonddeservice. Anciencalcul avaitcomptélesstatutsaprèsburst commeattentes :corrigé75→40demandes,bornehautehorsattentesmoyenne3,142850/max4,000225ms. Anciencalcul localpréservé,aucunretest.

8MHz conditionnel justifié :gain filestimé896µs/segment,21% dubudgetnominal, utile réservefuturs traitements maisFFT/stockage nonmesurés. T4intégritéétablie,maxIIS3DWB10MHz documenté, mêmeHSI16/core160/rails/câblage ;seuldiviseur4→2,guards/cleanup/ODR/parseridentiques. Uniqueimage8 compilée, pasencoreflashée ;aprèspublicationdececheckpoint,uneseulecapture4096,STOPtoutdéfaut. 8NONTESTÉ àcecheckpoint.

Aucune nouvellecapture4,S,2ou512 ;originaux/archivesS/T/worktrees/stashespréservés,primaire/M014-H/FRAM/SD/Modbus/HPexclus. Preuves danspreuves/TR2-20261013-M015-T-RESUME et /home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z. BLOCKED provisoire pourpublication/essai8conditionnel ;rapportfinalsuivra.
